#pragma once

namespace ROC
{

class CustomData
{
    union
    {
        bool m_bool;
        int m_int;
        double m_double;
        float m_float;
        void *m_ptr;
    };
    unsigned char m_type;
    std::string m_string;
public:
    enum CustomDataType : unsigned char
    {
        CDT_None = 0U,
        CDT_Nil,
        CDT_Boolean,
        CDT_Integer,
        CDT_Double,
        CDT_Float,
        CDT_String,
        CDT_Element
    };

    CustomData();
    CustomData(const CustomData& f_data);
    ~CustomData();

    inline unsigned char GetType() const { return m_type; }

    inline void SetNil() { m_type = CDT_Nil; }

    inline bool GetBoolean() const { return m_bool; }
    void SetBoolean(bool f_val);

    inline int GetInteger() const { return m_int; }
    void SetInteger(int f_val);

    inline double GetDouble() const { return m_double; }
    void SetDouble(double f_val);

    inline float GetFloat() const { return m_float; }
    void SetFloat(float f_val);

    void GetElement(void *&f_ptr, std::string &f_name) const;
    void SetElement(void *f_ptr, const std::string &f_name);

    inline const std::string& GetString() const { return m_string; }
    void SetString(const std::string &f_val);
    void SetString(const char *f_val, size_t f_size);
    inline void ClearString() { m_string.clear(); }

    CustomData& operator=(const CustomData &f_data);
};

}
