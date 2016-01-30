#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

namespace Math
{
	struct t_float2
	{
		t_float2(){}
		t_float2(float x, float y)
		{
			this->x = x;
			this->y = y;
		}

		union
		{
			struct
			{
				float x;
				float y;
			};

			glm::vec2 _glmVector;
		};
	};

	struct t_float3
	{
		t_float3(){}
		t_float3(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}

		union
		{
			struct
			{
				float x;
				float y;
				float z;
			};

			glm::vec3 _glmVector;
		};
	};

	struct t_float4
	{
		t_float4(){}
		t_float4(float x, float y, float z, float w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}

		union
		{
			struct
			{
				float x;
				float y;
				float z;
				float w;
			};

			glm::vec4 _glmVector;
		};
	};

	template<typename T, typename... S>
	struct Vector : public T
	{
		Vector(S... x) : T(x...){}
		Vector(){}

		Vector(const T& v)
		{
			_glmVector = v._glmVector;
		}

		// Comparision operators
		bool operator == (const Vector<T, S...>& v) const
		{
			return _glmVector == v;
		}

		bool operator != (const Vector<T, S...>& v) const
		{
			return _glmVector != v;
		}

		// Assignment operators
		Vector<T, S...>& operator= (const Vector<T, S...>& v) { _glmVector = v._glmVector; return *this; }
		Vector<T, S...>& operator+= (const Vector<T, S...>& v) { _glmVector += v._glmVector; return *this; }
		Vector<T, S...>& operator-= (const Vector<T, S...>& v) { _glmVector -= v._glmVector; return *this; }
		Vector<T, S...>& operator*= (const Vector<T, S...>& v) { _glmVector *= v._glmVector; return *this; }
		Vector<T, S...>& operator*= (float s) { _glmVector *= s; return *this; }
		Vector<T, S...>& operator/= (float s) { _glmVector *= s; return *this; }

		// Urnary operators
		Vector<T, S...> operator+ () const { return *this; }
		Vector<T, S...> operator- () const { return Vector<T, S...>(-x, -y); }

        //TODO: float = S ;)
		float length() const { return _glmVector.length(); }
		float lengthSquared() const { return x * x + y * y; }

        float dot(const Vector<T, S...>& v) const { glm::dot(_glmVector, v._glmVector); } //TODO: Degenerated: no PB style!
		void cross(const Vector<T, S...>& v, Vector<T, S...>& result) const { glm::cross(_glmVector, v._glmVector); }
		Vector<T, S...> cross(const Vector<T, S...>& v) const { _glmVector = glm::cross(_glmVector, v._glmVector); return *this; }

		void normalize() { _glmVector = glm::normalize(_glmVector); }

		// Static functions
		static float distance(const Vector<T, S...>& v1, const Vector<T, S...>& v2) { return (v2 - v1).length(); }
		static float distanceSquared(const Vector<T, S...>& v1, const Vector<T, S...>& v2) { return (v2 - v1).lengthSquared(); };
	};

	template<typename T, typename... S> Vector<T, S...> operator+ (const Vector<T, S...>& v1, const Vector<T, S...>& v2)
	{
		Vector<T, S...> r;
		r._glmVector = v1._glmVector + v2._glmVector;
		return r;
	}

	template<typename T, typename... S> Vector<T, S...> operator- (const Vector<T, S...>& v1, const Vector<T, S...>& v2)
	{
		Vector<T, S...> r;
		r._glmVector = v1._glmVector + v2._glmVector;
		return r;
	}

	template<typename T, typename... S> Vector<T, S...> operator* (const Vector<T, S...>& V, float s)
	{
		Vector<T, S...> r;
		r._glmVector = v1._glmVector * s;
		return r;
	}

	template<typename T, typename... S> Vector<T, S...> operator* (float s, const Vector<T, S...>& V)
	{
		Vector<T, S...> r;
		r._glmVector = s * v2._glmVector;
		return r;
	}

	typedef Vector<t_float2, float, float> float2;
	typedef Vector<t_float3, float, float, float> float3;
	typedef Vector<t_float4, float, float, float, float> float4;
	
	//------------------------------------------------------------------------------
	// 4x4 Matrix (assumes right-handed cooordinates)
	struct Matrix
	{
		Matrix(){}
		Matrix(const glm::mat4x4& m)
		{
			_glmMatrix = m;
		}

		Matrix(float m00, float m01, float m02, float m03,
			float m10, float m11, float m12, float m13,
			float m20, float m21, float m22, float m23,
			float m30, float m31, float m32, float m33)
		{
			m[0][0] = m00;
			m[0][1] = m01;
			m[0][2] = m02;
			m[0][3] = m03;

			m[1][0] = m10;
			m[1][1] = m11;
			m[1][2] = m12;
			m[1][3] = m13;

			m[2][0] = m20;
			m[2][1] = m21;
			m[2][2] = m22;
			m[2][3] = m23;

			m[3][0] = m30;
			m[3][1] = m31;
			m[3][2] = m32;
			m[3][3] = m33;
		}

		// Comparision operators
		bool operator == ( const Matrix& M ) const { return _glmMatrix == M._glmMatrix;}
		bool operator != ( const Matrix& M ) const { return _glmMatrix != M._glmMatrix;}

		// Assignment operators
		Matrix& operator= (const Matrix& M) { memcpy_s( this, sizeof(float)*16, &M, sizeof(float)*16); return *this; }
		Matrix& operator+= (const Matrix& M) { _glmMatrix += M._glmMatrix; return *this; }
		Matrix& operator-= (const Matrix& M) { _glmMatrix -= M._glmMatrix; return *this; }
		Matrix& operator*= (const Matrix& M) { _glmMatrix *= M._glmMatrix; return *this; }
		Matrix& operator*= (float s) { _glmMatrix *= s; return *this; }
		Matrix& operator/= (float s) { _glmMatrix *= s; return *this; }

		// Properties
		float3 Up() const { return float3( _21, _22, _23); }
		void Up( const float3& v ) { _21 = v.x; _22 = v.y; _23 = v.z; }

		float3 Down() const { return float3( -_21, -_22, -_23); }
		void Down( const float3& v ) { _21 = -v.x; _22 = -v.y; _23 = -v.z; }

		float3 Right() const { return float3( _11, _12, _13 ); }
		void Right( const float3& v ) { _11 = v.x; _12 = v.y; _13 = v.z; }

		float3 Left() const { return float3( -_11, -_12, -_13 ); }
		void Left( const float3& v ) { _11 = -v.x; _12 = -v.y; _13 = -v.z; }

		float3 Forward() const  { return float3( -_31, -_32, -_33 ); }
		void Forward( const float3& v ) { _31 = -v.x; _32 = -v.y; _33 = -v.z; }

		float3 Backward() const { return float3( _31, _32, _33 ); }
		void Backward( const float3& v ) { _31 = v.x; _32 = v.y; _33 = v.z; }

		float3 Translation() const { return float3( _41, _42, _43 ); }
		float3 TranslationT() const { return float3( _14, _24, _34 ); }
		void Translation( const float3& v ) { _41 = v.x; _42 = v.y; _43 = v.z; }

		Matrix Transpose() const {return glm::transpose(_glmMatrix); }

		Matrix Invert() const {return glm::inverse(_glmMatrix); }

		float Determinant() const  {return glm::determinant(_glmMatrix); }

		static Matrix CreateTranslation( const float3& position ){return Matrix(glm::translate(glm::mat4x4(), position._glmVector)); }
		static Matrix CreateTranslation( float x, float y, float z ){return Matrix(glm::translate(glm::mat4x4(), glm::vec3(x,y,z))); }

		static Matrix CreateScale( const float3& scales ) {return Matrix(glm::scale(glm::mat4x4(), scales._glmVector)); }
		static Matrix CreateScale( float xs, float ys, float zs ) {return Matrix(glm::scale(glm::mat4x4(), glm::vec3(xs,ys,zs))); }
		static Matrix CreateScale( float scale ) {return Matrix(glm::scale(glm::mat4x4(), glm::vec3(scale,scale,scale))); }

		static Matrix CreateRotationX( float radians ){return Matrix(glm::rotate(glm::mat4x4(), radians, glm::vec3(1,0,0))); }
		static Matrix CreateRotationY( float radians ){return Matrix(glm::rotate(glm::mat4x4(), radians, glm::vec3(0,1,0))); }
		static Matrix CreateRotationZ( float radians ){return Matrix(glm::rotate(glm::mat4x4(), radians, glm::vec3(0,0,1))); }

		static Matrix CreateFromAxisAngle( const float3& axis, float angle ) {return Matrix(glm::rotate(glm::mat4x4(), angle, axis._glmVector)); }

		static Matrix CreatePerspectiveFieldOfView( float fov, float width, float height, float nearPlane, float farPlane ) {return Matrix(glm::perspectiveFovRH(fov, width, height, nearPlane, farPlane)); }

		static Matrix CreateOrthographic( float left, float right, float bottom, float top ) {return Matrix(glm::ortho(left, right, bottom, top)); }

		static Matrix CreateLookAt( const float3& position, const float3& target, const float3& up ) {return Matrix(glm::lookAtRH(target._glmVector, position._glmVector, up._glmVector)); }

		// TODO: Implement this
		static Matrix CreateFromQuaternion( const float4& quat );

		union
		{
			struct
			{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};
			float m[4][4];
			glm::mat4x4 _glmMatrix;
		};
	};

	Matrix operator* (const Matrix& M1, const Matrix& M2)
	{
		return M1._glmMatrix * M2._glmMatrix;
	}
};
