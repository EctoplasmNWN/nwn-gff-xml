#include "FileFormats/Gff.hpp"
#include "tinyxml2.h"

using namespace FileFormats::Gff;
using namespace tinyxml2;

namespace {

void write_to_xml_r(const Friendly::GffStruct& element, XMLDocument* doc, XMLElement* root, bool top_level = false);

bool read_from_xml(const char* file, Friendly::Gff* out)
{
    return true;
}

bool read_from_gff(const char* file, Friendly::Gff* out)
{
    Raw::Gff gff_raw;
    if (!Raw::Gff::ReadFromFile(file, &gff_raw))
    {
        return false;
    }

    *out = Friendly::Gff(std::move(gff_raw));
    return true;
}

bool write_to_xml(const char* file, const Friendly::Gff* in)
{
    XMLDocument doc;
    XMLElement* root = doc.NewElement("Gff");
    root->SetAttribute("Version", 1);
    doc.InsertFirstChild(root);
    write_to_xml_r(in->GetTopLevelStruct(), &doc, root, true);
    return doc.SaveFile(file) == XML_SUCCESS;
}

bool write_to_gff(const char* file, const Friendly::Gff* in)
{
    return in->WriteToFile(file);
}

template <typename T>
XMLElement* create_generic_node(const char* type, const char* name, const T& value, XMLDocument* doc)
{
    XMLElement* new_node = doc->NewElement(type);
    new_node->SetText(value);
    new_node->SetAttribute("Name", name);
    return new_node;
}

void write_to_xml_r(const Friendly::GffStruct& element, XMLDocument* doc, XMLElement* root, bool top_level)
{
    XMLElement* new_root = top_level ? nullptr : doc->NewElement("Struct");

    if (new_root)
    {
        new_root->SetAttribute("Id", element.GetUserDefinedId());
        std::swap(root, new_root);
    }

    for (auto const& kvp : element.GetFields())
    {
        if (kvp.second.first == Raw::GffField::Type::BYTE)
        {
            Friendly::Type_BYTE value;
            element.ReadField(kvp, &value);
            root->InsertEndChild(create_generic_node("Byte", kvp.first.c_str(), value, doc));
        }
        else if (kvp.second.first == Raw::GffField::Type::CHAR)
        {
            Friendly::Type_CHAR value;
            element.ReadField(kvp, &value);
            root->InsertEndChild(create_generic_node("Char", kvp.first.c_str(), value, doc));
        }
        else if (kvp.second.first == Raw::GffField::Type::WORD)
        {
            Friendly::Type_WORD value;
            element.ReadField(kvp, &value);
            root->InsertEndChild(create_generic_node("Word", kvp.first.c_str(), value, doc));
        }
        else if (kvp.second.first == Raw::GffField::Type::SHORT)
        {
            Friendly::Type_SHORT value;
            element.ReadField(kvp, &value);
            root->InsertEndChild(create_generic_node("Short", kvp.first.c_str(), value, doc));
        }
        else if (kvp.second.first == Raw::GffField::Type::DWORD)
        {
            Friendly::Type_DWORD value;
            element.ReadField(kvp, &value);
            root->InsertEndChild(create_generic_node("DWord", kvp.first.c_str(), value, doc));
        }
        else if (kvp.second.first == Raw::GffField::Type::INT)
        {
            Friendly::Type_INT value;
            element.ReadField(kvp, &value);
            root->InsertEndChild(create_generic_node("Int", kvp.first.c_str(), value, doc));
        }
        else if (kvp.second.first == Raw::GffField::Type::DWORD64)
        {
            Friendly::Type_DWORD64 value;
            element.ReadField(kvp, &value);
            root->InsertEndChild(create_generic_node("DWord64", kvp.first.c_str(), value, doc));
        }
        else if (kvp.second.first == Raw::GffField::Type::INT64)
        {
            Friendly::Type_INT64 value;
            element.ReadField(kvp, &value);
            root->InsertEndChild(create_generic_node("Int64", kvp.first.c_str(), value, doc));
        }
        else if (kvp.second.first == Raw::GffField::Type::FLOAT)
        {
            Friendly::Type_FLOAT value;
            element.ReadField(kvp, &value);
            char flt[64];
            std::sprintf(flt, "%f", value);
            root->InsertEndChild(create_generic_node("Float", kvp.first.c_str(), flt, doc));
        }
        else if (kvp.second.first == Raw::GffField::Type::DOUBLE)
        {
            Friendly::Type_DOUBLE value;
            element.ReadField(kvp, &value);
            char dbl[64];
            std::sprintf(dbl, "%f", value);
            root->InsertEndChild(create_generic_node("Double", kvp.first.c_str(), dbl, doc));
        }
        else if (kvp.second.first == Raw::GffField::Type::CExoString)
        {
            Friendly::Type_CExoString value;
            element.ReadField(kvp, &value);
            root->InsertEndChild(create_generic_node("CExoString", kvp.first.c_str(), value.m_String.c_str(), doc));
        }
        else if (kvp.second.first == Raw::GffField::Type::ResRef)
        {
            Friendly::Type_CResRef value;
            element.ReadField(kvp, &value);
            char resref[32];
            std::sprintf(resref, "%.*s", value.m_Size, value.m_String);
            root->InsertEndChild(create_generic_node("Word", kvp.first.c_str(), resref, doc));
        }
        else if (kvp.second.first == Raw::GffField::Type::CExoLocString)
        {
            Friendly::Type_CExoLocString value;
            element.ReadField(kvp, &value);

            XMLElement* new_node = doc->NewElement("CExoLocString");
            new_node->SetAttribute("Name", kvp.first.c_str());
            new_node->InsertEndChild(create_generic_node("DWord", "StringRef", value.m_StringRef, doc));

            for (std::size_t i = 0; i < value.m_SubStrings.size(); ++i)
            {
                XMLElement* new_nod_substr = doc->NewElement("SubString");
                new_nod_substr->InsertEndChild(create_generic_node("DWord", "StringID", value.m_SubStrings[i].m_StringID, doc));
                new_nod_substr->InsertEndChild(create_generic_node("DWord", "String", value.m_SubStrings[i].m_String.c_str(), doc));
                new_node->InsertEndChild(new_nod_substr);
            }

            root->InsertEndChild(new_node);
        }
        else if (kvp.second.first == Raw::GffField::Type::VOID)
        {
            std::printf("VOID data detected in %s. Dropping.\n", kvp.first.c_str());
        }
        else if (kvp.second.first == Raw::GffField::Type::Struct)
        {
            Friendly::Type_Struct value;
            element.ReadField(kvp, &value);
            write_to_xml_r(value, doc, root);
        }
        else if (kvp.second.first == Raw::GffField::Type::List)
        {
            Friendly::Type_List value;
            element.ReadField(kvp, &value);

            XMLElement* new_node = doc->NewElement("List");
            new_node->SetAttribute("Name", kvp.first.c_str());

            for (const Friendly::GffStruct& struc : value.GetStructs())
            {
                write_to_xml_r(struc, doc, new_node);
            }

            root->InsertEndChild(new_node);
        }
        else
        {
            ASSERT_FAIL();
        }
    }

    if (new_root)
    {
        new_root->InsertEndChild(root);
    }
}

}

int main(int argc, char** argv)
{
    const char* path_out = argv[1];
    const char* path_in = argv[2];

    bool write_xml = std::strstr(path_out, ".xml") != nullptr;
    bool read_xml = std::strstr(path_in, ".xml") != nullptr;

    std::printf("Writing XML: %s.\n", write_xml ? "true" : "false");
    std::printf("Reading XML: %s.\n", read_xml ? "true" : "false");

    std::printf("Processing %s -> %s.\n", path_in, path_out);

    Friendly::Gff gff;

    if (bool read_success = read_xml ? read_from_xml(path_in, &gff) : read_from_gff(path_in, &gff); !read_success)
    {
        std::printf("Failed to read.\n");
        return 1;
    }

    if (bool write_success = write_xml ? write_to_xml(path_out, &gff) : write_to_gff(path_out, &gff); !write_success)
    {
        std::printf("Failed to write.\n");
        return 1;
    }

    return 0;
}
