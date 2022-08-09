/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

using namespace std;
namespace fs = filesystem;

namespace
{
///////////////////////////////////////////////////////////////////////////////////////////////////
string      PROGRAM_NAME; ///< We set the program name on this global early on for use in Usage.
string_view VERSION = "1.0.0";

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Supported extensions for the files in the input directory.
// clang-format off
constexpr string_view SHADER_EXTENSIONS[] =
{
  ".vert",
  ".frag",
  ".def"
};
// clang-format on

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Function & variable to retrieve the size of the extension with the largest string size.
constexpr auto GetShaderExtensionMaxSize()
{
  auto maxSize = 0u;
  for(const auto& extension : SHADER_EXTENSIONS)
  {
    if(extension.size() > maxSize)
    {
      maxSize = extension.size();
    }
  }
  return maxSize;
}
constexpr const int SHADER_MAX_EXTENSION_SIZE(GetShaderExtensionMaxSize());

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Prints out the Usage to standard output.
void Usage()
{
  cout << "Usage: " << PROGRAM_NAME << " [OPTIONS] [IN_DIR] [OUT_DIR]" << endl;
  cout << "  IN_DIR:  Input Directory which has all the shader files." << endl;
  cout << "           Supported extensions:";
  string extensions;
  for(const auto& extension : SHADER_EXTENSIONS)
  {
    extensions = extensions + " \"" + string(extension) + "\",";
  }
  extensions.pop_back(); // Remove the last comma.
  cout << extensions << "." << endl;
  cout << "  OUT_DIR: Directory where the generated shader source code will be outputted to." << endl;
  cout << "           This directory will be created if it does not exist." << endl;
  cout << "           Any existing files of the same name in the directory will be overwritten." << endl;
  cout << "  Options: " << endl;
  cout << "     -s|--skip     Skips the generation of the built-in header and source files" << endl;
  cout << "     -v|--version  Prints out the version" << endl;
  cout << "     -h|--help     Help" << endl;
  cout << "  NOTE: The options can be placed after the IN_DIR & OUT_DIR as well" << endl;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Uses the filename to generate the shader variable name to use in source code.
/// @param[in]  filename  The filename of the shader (including the extension)
/// @return The shader variable name
string GetShaderVariableName(const string& filename)
{
  string shaderVariableName("SHADER_" + filename);
  for_each(
    shaderVariableName.begin(),
    shaderVariableName.end(),
    [](char& character) {
      switch(character)
      {
        case '-':
        case '.':
        {
          character = '_';
          break;
        }

        default:
        {
          character = ::toupper(character);
          break;
        }
      }
    });
  return shaderVariableName;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Uses the ourDir & filename to generate the path of the output header file for the shader.
/// @param[in]  outDir    The directory where the readable shaders will be outputted to
/// @param[in]  filename  The filename of the shader (including the extension)
/// @return The path to the output file
fs::path GetShaderOutputFilePath(fs::path& outDir, const string& filename)
{
  string outFilename(filename);
  replace(outFilename.end() - SHADER_MAX_EXTENSION_SIZE, outFilename.end(), '.', '-');
  outFilename = outDir.string() + "/" + outFilename + ".h";
  return outFilename;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Generates the header file from the input shader file.
/// @param[in]  shaderFile          The full path of the input shader file
/// @param[in]  shaderVariableName  The variable name to use for the string_view
/// @param[in]  outFilePath         The full path to the output file
void GenerateHeaderFile(
  ifstream&       shaderFile,
  const string&   shaderVariableName,
  const fs::path& outFilePath)
{
  cout << "  Generating \"" << shaderVariableName << "\" in " << outFilePath.filename();
  ofstream outFile(outFilePath);
  if(outFile.is_open())
  {
    outFile << "#pragma once" << endl
            << endl;
    outFile << "const std::string_view " << shaderVariableName << endl;
    outFile << "{" << endl;

    // Using Raw String Literal to generate shader files as this will simplify the file layout.
    // And it will fix some compilation warnings about missing terminating strings.
    // Note : we should skip empty headline to guarantee that "#version ~~~" as top of shader code.
    outFile << "R\"(";
    string line;
    bool   firstLinePrinted = false;
    while(getline(shaderFile, line))
    {
      if(!firstLinePrinted && line.find_first_not_of(" \t\r\n") == std::string::npos)
      {
        // Empty string occured!
        continue;
      }
      firstLinePrinted = true;
      outFile << line << endl;
    }
    outFile << ")\"" << endl;
    outFile << "};" << endl;
    cout << " [OK]" << endl;
  }
  else
  {
    cout << " [FAIL]" << endl;
  }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/// If required, this accumulates data about all the shaders & generates the built-in cpp & header
class BuiltInFilesGenerator
{
public:
  /// Constructor
  /// @param[in]  outDir  The path to the output directory
  BuiltInFilesGenerator(const fs::path& outDir)
  : mHeaderFilePath(outDir.string() + "/../" + string(HEADER_FILE_NAME)),
    mSourceFilePath(outDir.string() + "/" + string(SOURCE_FILE_NAME))
  {
  }

  /// Default destructor
  ~BuiltInFilesGenerator() = default;

  /// Adds the variable and the header file name to the appropriate vectors.
  /// @param[in]  variableName    The string_view variable name used
  /// @param[in]  headerFileName  The name of the header used
  void Add(string&& variableName, const std::string& headerFilename)
  {
    mVariableNames.emplace_back(variableName);
    mHeaderFileNames.emplace_back(headerFilename);
  }

  // Generates the built in files.
  void Generate()
  {
    GenerateFile(
      mVariableNames,
      mHeaderFilePath,
      "#pragma once\n\n#include <string_view>\n\n",
      "extern const std::string_view ",
      ";");

    GenerateFile(
      mHeaderFileNames,
      mSourceFilePath,
      "#include \"../" + string(HEADER_FILE_NAME) + "\"\n\n",
      "#include \"",
      "\"");
  }

private:
  /// Generates the required file.
  /// @param[in]  strings   A reference to the vector to parse
  /// @param[in]  filePath  Outputs the data to this file
  /// @param[in]  header    Puts this before parsing any of the vector
  /// @param[in]  before    For each string, puts this string before it on every line
  /// @param[in]  after     For each string, puts this string after it on every line
  void GenerateFile(
    vector<string>&   strings,
    const string&     filePath,
    const string_view header,
    const string_view before,
    const string_view after)
  {
    sort(strings.begin(), strings.end());
    cout << "  Generating \"" << filePath << "\"";
    ofstream outFile(filePath);
    if(outFile)
    {
      outFile << header;
      for(auto& current : strings)
      {
        outFile << before << current << after << endl;
      }
      cout << " [OK]" << endl;
    }
    else
    {
      cout << " [FAIL]" << endl;
    }
  }

  constexpr static string_view HEADER_FILE_NAME = "builtin-shader-extern-gen.h";
  constexpr static string_view SOURCE_FILE_NAME = "builtin-shader-gen.cpp";

  const string   mHeaderFilePath;  ///< Path to the header file to generate
  const string   mSourceFilePath;  ///< Path to the source file to generate
  vector<string> mVariableNames;   ///< Holds all the variable names added through Add
  vector<string> mHeaderFileNames; ///< Holds all the header file names added through Add
};

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Generates the header files from the shaders in the input directory & built-in files if reqruied.
///
/// @param[in]  inDir                 The directory where all the input shader source is
/// @param[in]  outDir                The directory where the readable shaders will be outputted to
/// @param[in]  generateBuiltInFiles  If true, we generate the built-in files as well
/// @return 0 if successful, 1 if failure
int GenerateShaderSources(fs::path inDir, fs::path outDir, const bool generateBuiltInFiles)
{
  if(!fs::is_directory(inDir))
  {
    cerr << "ERROR: " << inDir << " is not a valid directory" << endl;
    Usage();
    return 1;
  }

  try
  {
    fs::create_directories(outDir);
  }
  catch(...)
  {
    cerr << "ERROR: Unable to create directory " << outDir << endl;
    return 1;
  }

  cout << "====================================================================" << endl;
  cout << "Shader Input Directory:  " << inDir << endl;
  cout << "Shader Output Directory: " << outDir << endl;
  cout << "====================================================================" << endl;

  BuiltInFilesGenerator generator(outDir);

  for(auto& file : fs::directory_iterator(inDir))
  {
    if(file.is_regular_file())
    {
      for(const auto& extension : SHADER_EXTENSIONS)
      {
        if(file.path().extension() == extension)
        {
          const fs::path& path(file.path());
          const string    filename(path.filename().string());
          string          shaderVariableName(GetShaderVariableName(filename));
          ifstream        shaderFile(path);
          if(shaderFile.is_open())
          {
            fs::path outFilePath(GetShaderOutputFilePath(outDir, filename));
            GenerateHeaderFile(shaderFile, shaderVariableName, outFilePath);
            generator.Add(std::move(shaderVariableName), outFilePath.filename().string());
          }
          break;
        }
      }
    }
  }

  if(generateBuiltInFiles)
  {
    generator.Generate();
  }

  cout << "====================================================================" << endl;
  return 0;
}

} // unnamed namespace

///////////////////////////////////////////////////////////////////////////////////////////////////
/// MAIN.
int main(int argc, char* argv[])
{
  PROGRAM_NAME = argv[0];

  bool generateBuiltInFiles = true;

  string inDir;
  string outDir;

  for(auto i = 1; i < argc; ++i)
  {
    string option(argv[i]);
    if(option == "--skip" || option == "-s")
    {
      generateBuiltInFiles = false;
    }
    else if(option == "--help" || option == "-h")
    {
      cout << "DALi Shader Generator v" << VERSION << endl
           << endl;
      Usage();
      return 0;
    }
    else if(option == "--version" || option == "-v")
    {
      cout << VERSION << endl;
      return 0;
    }
    else if(*option.begin() == '-')
    {
      cerr << "ERROR: " << option << " is not a supported option" << endl;
      Usage();
      return 1;
    }
    else if(inDir.empty())
    {
      inDir = option;
    }
    else if(outDir.empty())
    {
      outDir = option;
    }
    else if(inDir.size() && outDir.size())
    {
      cerr << "ERROR: Too many options" << endl;
      Usage();
      return 1;
    }
  }

  if(inDir.empty() || outDir.empty())
  {
    cerr << "ERROR: Both IN_DIR & OUT_DIR not provided" << endl;
    Usage();
    return 1;
  }

  return GenerateShaderSources(inDir, outDir, generateBuiltInFiles);
}
