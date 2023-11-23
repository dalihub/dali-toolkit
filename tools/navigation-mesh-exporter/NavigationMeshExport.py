import bpy
import struct
import ctypes
import tempfile
import os
import mathutils
from mathutils import Matrix
from mathutils import Vector

def MakeVersion( maj, min ):
    return ctypes.c_uint32((ctypes.c_uint32(maj).value << 16) | ctypes.c_uint32(min).value).value

EXPORT_VERSION = MakeVersion( 1, 0 )

# If BAKE_TRANSFORM is True the object transform is baked into vertices
BAKE_TRANSFORM = False

CHECKSUM = "NAVM" # 4 bytes of checksum

# From GLTF addon
def to_yup():
    """Transform to Yup."""
    return Matrix(
        ((1.0, 0.0, 0.0, 0.0),
         (0.0, 0.0, 1.0, 0.0),
         (0.0, -1.0, 0.0, 0.0),
         (0.0, 0.0, 0.0, 1.0))
    )

def to_yup3():
    """Transform to Yup."""
    return Matrix(
        ((1.0, 0.0, 0.0),
         (0.0, 0.0, 1.0),
         (0.0, -1.0, 0.0))
    )

# struct NavMeshHeader
# version 10
# The script writes multiple binary buffers which are concatenated at the end once
# the offsets are resolved
class NavMeshHeader10:

    # fields of NavMeshHeader

    def __init__(self, outputPath):
        self.outputPath = outputPath
        self.meshObject = bpy.context.selected_objects[0]

        self.checksum = CHECKSUM
        self.version = EXPORT_VERSION

        # fields of version
        self.dataOffset = 0

        self.vertexCount = 0
        self.vertexDataOffset = 0

        self.edgeCount = 0
        self.edgeDataOffset = 0

        self.polyCount = 0
        self.polyDataOffset = 0

        self.mesh = self.meshObject.data

        # helper fields
        self.edgeBufferSize = 0
        self.vertexBufferSize = 0
        self.polyBufferSize = 0
        self.bufferHeaderSize = 0
        self.endianness = 0 # TODO

        with tempfile.TemporaryDirectory() as temp_dir:
            self.tempDir = temp_dir

        print(temp_dir)
        os.makedirs(temp_dir, exist_ok=True)
        return

    def Process(self):

        # the data offset will be filled at later stage
        self.polyCount = len(self.mesh.polygons)
        self.vertexCount = len(self.mesh.vertices)
        self.edgeCount = len(self.mesh.edges)
        return

    # vertices format
    # N x ( f, f, f ), indexed from 0
    def WriteVertices(self, bakeTransform=True):
        out = open(os.path.join(self.tempDir, "myvertex-vertices-nt.bin"), "wb")
        i = 0
        for vert in self.mesh.vertices:
            fmt = '@fff'
            co = vert.co.copy()
            # for now bake transform into the mesh (it has to be aligned properly)
            if bakeTransform:
                print('baking')
                co = self.meshObject.matrix_world @ Vector(co)

            out.write(struct.pack(fmt, co[0], co[1], co[2]))
            print(f'i: {i:d}: {co[0]:f}, {co[1]:f}, {co[2]:f}')
            i += 1
        self.vertexBufferSize = out.tell()
        out.close()
        return

    # This function builds vertex map after finding duplicate vertices
    # it is essential to do it as two or more vertices may appear as one
    # in the editor, yet they may be defined as multiple vertices due to carrying
    # different UVs or other attributes. NavMesh needs only coordinates and
    # two vertices with the same coordinates are considered as one.
    def CreateEdgeMap(self):
        mesh = self.mesh
        vertexBuffer = {}
        i = 0
        for v in mesh.vertices:
            # using string of coordinates as a key
            key = f'{v.co}'

            if key in vertexBuffer:
                vertexBuffer[key].append(i)
            else:
                vertexBuffer[key] = [i]
            i += 1

        # build substitute array
        # value at each index will be either own (no substitute)
        # or of another vertex so the keys of edges can be substituted

        vertexSubstitute = []
        for v in mesh.vertices:
            key = f'{v.co}'
            verts = vertexBuffer[key]

            # store always only first index of vertex
            vertexSubstitute.append(verts[0])

        #for i in range(0, len(vertexSubstitute)):
        #    print(f'{i} = {vertexSubstitute[i]}')

        # build edge map
        # we need to remap edge keys so we can point out to a single edge
        # end remove duplicates
        # Blender edge key is made of vertex indices sorted
        origKeys = []
        for e in mesh.edges:
            v0 = e.vertices[0]
            v1 = e.vertices[1]
            key = [v0, v1]
            key.sort()
            origKeys.append(key)
            #print(key)

        # create new keys by using vertex substitutes
        keyMap = {}
        edgeMap = {}
        edgeIndex = 0

        for key in origKeys:
            newKey = [vertexSubstitute[key[0]], vertexSubstitute[key[1]]]
            newKey.sort()

            # turn newkey into hashable string
            newKeyStr = f'{newKey}'

            #print(f'{key} -> {newKey}')

            if newKeyStr in keyMap:
                keyMap[newKeyStr].append(edgeIndex)
            else:
                keyMap[newKeyStr] = [edgeIndex]

            # turn original key into string
            edgeMap[f'{key}'] = keyMap[newKeyStr][0]
            edgeIndex += 1

        return edgeMap


    """
    Edge data stores:
    Pair of vertices (by index)
    Pair of polygons (by index)

    The polygon index is stored as uint16_t.
    It is possible that the edge belongs to a single polygon. In such case,
    the maximum of 0xFFFFF is stored as an index.
    The maximum index of polygon is 0xFFFE (65534).

    The pair of polygons are stored in order to speed up lookup of adjacent
    floor faces.
    """
    # N x ( f, f, f ), indexed from 0
    def WriteEdges(self):
        out = open(os.path.join(self.tempDir, "myvertex-edges.bin"), "wb")

        # add vertices to the edge
        edgeFaceList = []
        for edge in self.mesh.edges:
            edgeFaceList.append([])
            edgeFaceList[-1].append(edge.vertices[0])
            edgeFaceList[-1].append(edge.vertices[1])

        edgeMap = self.CreateEdgeMap()

        # for each polygon update the edges
        for poly in self.mesh.polygons:
            for ek in poly.edge_keys:
                ekArray = [ ek[0], ek[1] ]
                edgeIndexKey = f'{ekArray}'
                edgeIndex = edgeMap[edgeIndexKey]
                edgeFaceList[edgeIndex].append(poly.index)

        # make sure each edge contains 4 items
        for edge in edgeFaceList:
            if len(edge) == 2:
                edge.append( 0xffff )
            if len(edge) == 3:
                edge.append( 0xffff )
        count = 0
        for edge in edgeFaceList:
            if len(edge) == 4:
                count += 1
        if count == len(edgeFaceList):
            print("all correct!")
        # save the data into file
        fmt = '@HHHH'
        i = 0
        for edge in edgeFaceList:
            out.write(struct.pack(fmt, edge[0], edge[1], edge[2], edge[3]))
            print(f'i: {i:d}: {edge[0]:d}, {edge[1]:d}, {edge[2]:d}, {edge[3]:d}')
            i += 1

        self.edgeBufferSize = out.tell()

        out.close()
        return

    ###################################################################
    """
    Writes buffer of polygons. Polys must be triangles.
    The data contains:
    - vertex indices 3*u16
    - edge indices   3*u16
    - normal vector of poly 3*f32
    - center 3*f32 (we may need center point for navigation)
    """
    def WritePolys(self):
        out = open(os.path.join(self.tempDir, "myvertex-polys.bin"), "wb")

        # for each polygon update the edges

        polyIndex = 0

        edgeMap = self.CreateEdgeMap()

        for poly in self.mesh.polygons:
            edges = []
            for ek in poly.edge_keys:
                ekArray = [ ek[0], ek[1] ]
                edgeIndexKey = f'{ekArray}'
                edges.append(edgeMap[edgeIndexKey])

            verts = poly.vertices
            norm = poly.normal.copy()

            # transform
            print(f'{norm.x:f}, {norm.y:f}, {norm.z:f}')

            center = poly.center

            # calculate normal

            fmt='@HHHHHHffffff'

            out.write(struct.pack(fmt,
                                  verts[0], verts[1], verts[2],
                                  edges[0], edges[1], edges[2],
                                  norm.x, norm.y, norm.z,
                                  center.x, center.y, center.z))

            print('{edges[0]}, {edges[1]}, {edges[2]}')
        self.polyBufferSize = out.tell()
        out.close()
        return

    def WriteBinary(self):

        out = open(self.outputPath, "wb")

        # write common header fields
        c = bytes(CHECKSUM, "ascii")
        csum = ctypes.c_uint32( (c[3] << 24) | (c[2] << 16) | (c[1] << 8) | c[0] ).value
        fmt = "@II"
        out.write(struct.pack(fmt, csum, self.version))

        # write remaining fields
        fmt = '@IIIIIIIfff'
        gravity = bpy.context.scene.gravity.copy().normalized()
        out.write(struct.pack(fmt,
                              self.dataOffset,
                              self.vertexCount,
                              self.vertexDataOffset,
                              self.edgeCount,
                              self.edgeDataOffset,
                              self.polyCount,
                              self.polyDataOffset,
                              gravity.x, gravity.y, gravity.z
                              ))

        self.bufferHeaderSize = out.tell()

        out.close()

        return

    def Finalize(self):

        self.dataOffset = self.bufferHeaderSize
        self.vertexDataOffset = 0 # relative to data offset
        self.edgeDataOffset = self.vertexBufferSize
        self.polyDataOffset = self.edgeDataOffset + self.edgeBufferSize

        # write header again
        self.WriteBinary()

        # concatenate files

        with open(self.outputPath, "ab") as out, open(os.path.join(self.tempDir, "myvertex-vertices-nt.bin"), "rb") as file2:
            out.write(file2.read())

        with open(self.outputPath, "ab") as out, open(os.path.join(self.tempDir, "myvertex-edges.bin"), "rb") as file2:
            out.write(file2.read())

        with open(self.outputPath, "ab") as out, open(os.path.join(self.tempDir, "myvertex-polys.bin"), "rb") as file2:
            out.write(file2.read())

        return


################################################################
# UI

# Navigation Mesh Exporter
class OBJECT_OT_CustomOperator(bpy.types.Operator):
    bl_idname = "object.custom_operator"
    bl_label = "Export Navigation Mesh"

    def execute(self, context):
        bpy.ops.export.some_data('INVOKE_DEFAULT')
        return {'FINISHED'}

def submenu_func(self, context):
    layout = self.layout
    layout.operator("object.custom_operator")

def menu_func(self, context):
    layout = self.layout

    # Add a submenu
    layout.menu("VIEW3D_MT_custom_submenu", text="DALi")

class CustomSubmenu(bpy.types.Menu):
    bl_idname = "VIEW3D_MT_custom_submenu"
    bl_label = "DALi"

    def draw(self, context):
        layout = self.layout
        layout.operator("object.custom_operator")

def register():
    bpy.utils.register_class(OBJECT_OT_CustomOperator)
    bpy.types.VIEW3D_MT_object_context_menu.append(menu_func)
    bpy.utils.register_class(CustomSubmenu)
    bpy.utils.register_class(ExportSomeData)
def unregister():
    bpy.utils.unregister_class(OBJECT_OT_CustomOperator)
    bpy.types.VIEW3D_MT_object_context_menu.remove(menu_func)
    bpy.utils.unregister_class(CustomSubmenu)
    bpy.utils.unregister_class(ExportSomeData)

# exporter
class ExportSomeData(bpy.types.Operator):
    bl_idname = "export.some_data"
    bl_label = "Export DALi Navigation Mesh"

    filepath: bpy.props.StringProperty(subtype="FILE_PATH")

    @classmethod
    def poll(cls, context):
        return context.object is not None

    def execute(self, context):
        print(self.filepath)
        navmesh = NavMeshHeader10( self.filepath )
        navmesh.Process()
        navmesh.WriteBinary()
        navmesh.WriteVertices( False )
        navmesh.WriteEdges()
        navmesh.WritePolys()
        navmesh.Finalize()
        return {'FINISHED'}

    def invoke(self, context, event):
        context.window_manager.fileselect_add(self)
        return {'RUNNING_MODAL'}

if __name__ == "__main__":
    register()
