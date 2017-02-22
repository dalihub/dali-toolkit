/*
 * Copyright (c) 2017 Samsung Electronics Co., Ltd.
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

%rename(Rotation) Dali::Quaternion;
%rename(GetAxisAngle) Dali::Quaternion::ToAxisAngle;

%ignore Dali::Quaternion::Quaternion( Radian pitch, Radian yaw, Radian roll );
%ignore Dali::Quaternion::Quaternion(const Matrix& matrix);
%ignore Dali::Quaternion::Quaternion( const Vector4& vector );
%ignore Dali::Quaternion::Quaternion( const Vector3& v0, const Vector3& v1 );
%ignore Dali::Quaternion::Quaternion( const Vector3& xAxis, const Vector3& yAxis, const Vector3& zAxis );
%ignore Dali::Quaternion::Quaternion( float cosThetaBy2, float iBySineTheta, float jBySineTheta, float kBySineTheta );
%ignore Dali::Quaternion::SetEuler( Radian pitch, Radian yaw, Radian roll );
%ignore Dali::Quaternion::EulerAngles() const;
%ignore Dali::Quaternion::AsVector() const;
%ignore Dali::Quaternion::Rotate( const Vector4& vector ) const;
%ignore Dali::Quaternion::Rotate( const Vector3& vector ) const;
%ignore Dali::Quaternion::mVector;

%csmethodmodifiers Dali::Quaternion::operator+ "private";
%csmethodmodifiers Dali::Quaternion::operator- "private";
%csmethodmodifiers Dali::Quaternion::operator* "private";
%csmethodmodifiers Dali::Quaternion::operator/ "private";
%csmethodmodifiers Dali::Quaternion::operator+= "private";
%csmethodmodifiers Dali::Quaternion::operator-= "private";
%csmethodmodifiers Dali::Quaternion::operator*= "private";
%csmethodmodifiers Dali::Quaternion::operator/= "private";
%csmethodmodifiers Dali::Quaternion::operator== "private";
%csmethodmodifiers Dali::Quaternion::operator!= "private";
