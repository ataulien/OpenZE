#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <string.h>
#include <iostream>
#include <string>

namespace Math
{    
    constexpr int64_t ipow(int64_t base, int exp, int64_t result = 1)
    {
      return exp < 1 ? result : ipow(base * base, exp / 2, (exp % 2) ? result * base : result);
    }


    /**
     * @brief Converson of degrees to radians
     */
    static float DegToRad(float deg)
    {
        return glm::radians(deg);
    }

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

            glm::vec2 _glmt_vector;
        };

		friend std::ostream& operator<< (std::ostream &out, t_float2 &v);
    };

	static std::ostream& operator<< (std::ostream &out, t_float2 &v)
	{
		out << "[" + std::to_string(v.x) + ", " + std::to_string(v.y) + "]";
		return out;
	}

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

            glm::vec3 _glmt_vector;
        };

		friend std::ostream& operator<< (std::ostream &out, t_float3 &v);
    };

	static std::ostream& operator<< (std::ostream &out, t_float3 &v)
	{
		out << "[" + std::to_string(v.x) 
			+ ", " + std::to_string(v.y)
			+ ", " + std::to_string(v.z) + "]";
		return out;
	}

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

            glm::vec4 _glmt_vector;
        };

		friend std::ostream& operator<< (std::ostream &out, t_float4 &v);
    };

	static std::ostream& operator<< (std::ostream &out, t_float4 &v)
	{
		out << "[" + std::to_string(v.x) 
			+ ", " + std::to_string(v.y)
			+ ", " + std::to_string(v.z)
			+ ", " + std::to_string(v.w) + "]";
		return out;
	}

    template<typename T, typename... S>
    struct t_vector : public T
    {
        t_vector(S... x) : T(x...){}
        t_vector(){}

        t_vector(const T& v)
        {
            T::_glmt_vector = v._glmt_vector;
        }

        // Comparision operators
        bool operator == (const t_vector<T, S...>& v) const
        {
            return T::_glmt_vector == v;
        }

        bool operator != (const t_vector<T, S...>& v) const
        {
            return T::_glmt_vector != v;
        }

        // Assignment operators
        t_vector<T, S...>& operator= (const t_vector<T, S...>& v) { T::_glmt_vector = v._glmt_vector; return *this; }
        t_vector<T, S...>& operator+= (const t_vector<T, S...>& v) { T::_glmt_vector += v._glmt_vector; return *this; }
        t_vector<T, S...>& operator-= (const t_vector<T, S...>& v) { T::_glmt_vector -= v._glmt_vector; return *this; }
        t_vector<T, S...>& operator*= (const t_vector<T, S...>& v) { T::_glmt_vector *= v._glmt_vector; return *this; }
        t_vector<T, S...>& operator*= (float s) { T::_glmt_vector *= s; return *this; }
        t_vector<T, S...>& operator/= (float s) { T::_glmt_vector *= s; return *this; }

        // Urnary operators
        t_vector<T, S...> operator+ () const { return *this; }
        t_vector<T, S...> operator- () const { return t_vector<T, S...>(-T::x, -T::y); }

        //TODO: float = S ;)
        float length() const { return static_cast<float>(T::_glmt_vector.length()); }
        float lengthSquared() const { return T::x * T::x + T::y * T::y; }

        float dot(const t_vector<T, S...>& v) const { return glm::dot(T::_glmt_vector, v._glmt_vector); }
        void cross(const t_vector<T, S...>& v, t_vector<T, S...>& result) const { glm::cross(T::_glmt_vector, v._glmt_vector); }
        //t_vector<T, S...> cross(const t_vector<T, S...>& v) { T::_glmt_vector = glm::cross(T::_glmt_vector, v._glmt_vector); return *this; }

		t_vector<T, S...> normalize() { T::_glmt_vector = glm::normalize(T::_glmt_vector); return *this; }

        // Static functions
        static float distance(const t_vector<T, S...>& v1, const t_vector<T, S...>& v2) { return (v2 - v1).length(); }
        static float distanceSquared(const t_vector<T, S...>& v1, const t_vector<T, S...>& v2) { return (v2 - v1).lengthSquared(); };
		static t_vector<T, S...> cross(const t_vector<T, S...>& v1, const t_vector<T, S...>& v2) 
		{ 
			t_vector<T, S...> r;
			r._glmt_vector = glm::cross(v1._glmt_vector, v2._glmt_vector);
			return r; 
		}
    };

    template<typename T, typename... S> t_vector<T, S...> operator+ (const t_vector<T, S...>& v1, const t_vector<T, S...>& v2)
    {
        t_vector<T, S...> r;
        r._glmt_vector = v1._glmt_vector + v2._glmt_vector;
        return r;
    }

    template<typename T, typename... S> t_vector<T, S...> operator- (const t_vector<T, S...>& v1, const t_vector<T, S...>& v2)
    {
        t_vector<T, S...> r;
        r._glmt_vector = v1._glmt_vector - v2._glmt_vector;
        return r;
    }

    template<typename T, typename... S> t_vector<T, S...> operator* (const t_vector<T, S...>& v1, float s)
    {
        t_vector<T, S...> r;
        r._glmt_vector = v1._glmt_vector * s;
        return r;
    }

    template<typename T, typename... S> t_vector<T, S...> operator* (float s, const t_vector<T, S...>& v2)
    {
        t_vector<T, S...> r;
        r._glmt_vector = s * v2._glmt_vector;
        return r;
    }

    typedef t_vector<t_float2, float, float> float2;
    typedef t_vector<t_float3, float, float, float> float3;
    typedef t_vector<t_float4, float, float, float, float> float4;

    //------------------------------------------------------------------------------
    // 4x4 Matrix (assumes right-handed cooordinates)
    struct Matrix
    {
        Matrix(){}

		Matrix(float* pm)
		{
			memcpy(m, pm, sizeof(m));
		}

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
        Matrix& operator= (const Matrix& M) { memcpy( this, &M, sizeof(float)*16); return *this; }
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

		static Matrix CreateIdentity(){return Matrix(glm::mat4(1.0)); }

        static Matrix CreateTranslation( const float3& position ){return Matrix(glm::translate(glm::mat4x4(), position._glmt_vector)); }
        static Matrix CreateTranslation( float x, float y, float z ){return Matrix(glm::translate(glm::mat4x4(), glm::vec3(x,y,z))); }

        static Matrix CreateScale( const float3& scales ) {return Matrix(glm::scale(glm::mat4x4(), scales._glmt_vector)); }
        static Matrix CreateScale( float xs, float ys, float zs ) {return Matrix(glm::scale(glm::mat4x4(), glm::vec3(xs,ys,zs))); }
        static Matrix CreateScale( float scale ) {return Matrix(glm::scale(glm::mat4x4(), glm::vec3(scale,scale,scale))); }

        static Matrix CreateRotationX( float radians ){return Matrix(glm::rotate(glm::mat4x4(), radians, glm::vec3(1,0,0))); }
        static Matrix CreateRotationY( float radians ){return Matrix(glm::rotate(glm::mat4x4(), radians, glm::vec3(0,1,0))); }
        static Matrix CreateRotationZ( float radians ){return Matrix(glm::rotate(glm::mat4x4(), radians, glm::vec3(0,0,1))); }

        static Matrix CreateFromAxisAngle( const float3& axis, float angle ) {return Matrix(glm::rotate(glm::mat4x4(), angle, axis._glmt_vector)); }

        static Matrix CreatePerspectiveGL( float fov_deg, float width, float height, float nearPlane, float farPlane )
        {
            return Matrix(glm::perspectiveFovLH(fov_deg, width, height, nearPlane, farPlane));
        }

        static Matrix CreatePerspectiveDX( float fov_deg, float width, float height, float nearPlane, float farPlane )
        {
			return Matrix(glm::perspectiveFovLH(fov_deg, width, height, nearPlane, farPlane));

            const float fov_rad = glm::radians<float>(fov_deg);
            const float aspectRatio = width / height;
            const float yScale = 1 / tan(fov_rad / 2.0f);
            const float xScale = yScale / aspectRatio;

            glm::mat4x4 ret(0.0f);

			ret[0][0] = xScale;
			ret[1][1] = yScale;
			ret[2][2] = farPlane / (nearPlane - farPlane);
			ret[2][3] = -1;
			ret[3][2] = nearPlane * farPlane / (nearPlane - farPlane);

            return Matrix(ret);
        }


        static Matrix CreateOrthographic( float left, float right, float bottom, float top ) {return Matrix(glm::ortho(left, right, bottom, top)); }

        static Matrix CreateLookAt( const float3& position, const float3& target, const float3& up ) {return Matrix(glm::lookAt(position._glmt_vector, target._glmt_vector, up._glmt_vector)); }

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
			float mv[16];
            glm::mat4x4 _glmMatrix;
        };

		friend std::ostream& operator<< (std::ostream &out, Matrix &v);
    };

	static std::ostream& operator<< (std::ostream &out, Matrix &m)
	{
		out << "[";
		for(size_t i = 0; i < 16; i++)
		{
			out << std::to_string(m.mv[i]);

			// Only add "," when not at the last value
			if(i != 15)
				out << ", ";
		}
		out << "]";
		return out;
	}

    static Matrix operator* (const Matrix& M1, const Matrix& M2)
    {
        return M1._glmMatrix * M2._glmMatrix;
    }
};
