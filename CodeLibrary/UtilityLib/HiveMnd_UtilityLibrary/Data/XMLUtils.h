#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace HiveMnd::Data
{
    // =========================================================================
    // STRUCT : XMLNode
    // PURPOSE: Represents a single XML element (tag) that can store text and
    //          contain other nested XMLNode objects as children.
    // NOTES  : Keeps things lightweight and easy to work with for small files.
    // =========================================================================
    struct XMLNode
    {
        std::string name;                  // The tag name (ex: "Config" or "User")
        std::string value;                 // The text content inside the tag
        std::vector<XMLNode> children;     // Nested child tags (ex: <Settings> inside <Config>)

        // ---------------------------------------------------------------------
        // Function: FindChild
        // Purpose : Searches this node's children for one with a matching name.
        // Returns : Pointer to the found child or nullptr if not found.
        // ---------------------------------------------------------------------
        XMLNode* FindChild(const std::string& nodeName)
        {
            for (auto& child : children)
                if (child.name == nodeName)
                    return &child;
            return nullptr; // No child with that name
        }
    };

    // =========================================================================
    // CLASS : XMLUtils
    // PURPOSE: Provides helper methods to load, parse, and save simple XML data.
    // NOTES  : This is a *minimal* XML utility – works well for config files,
    //          saves, or structured settings, but not a full XML parser.
    // =========================================================================
    class XMLUtils
    {
    public:

        // ---------------------------------------------------------------------
        // Function: ParseXML
        // Purpose : Converts XML text into a hierarchy of XMLNode objects.
        // Params  : xmlText - The full XML string.
        // Returns : A root XMLNode containing all parsed tags and data.
        // ---------------------------------------------------------------------
        static XMLNode ParseXML(const std::string& xmlText)
        {
            std::istringstream stream(xmlText);
            return ParseNode(stream);
        }

        // ---------------------------------------------------------------------
        // Function: LoadXML
        // Purpose : Reads XML data directly from a file, then parses it.
        // Params  : filePath - Path to the XML file to read.
        // Returns : A parsed XMLNode tree representing the file's structure.
        // ---------------------------------------------------------------------
        static XMLNode LoadXML(const std::string& filePath)
        {
            std::ifstream file(filePath);
            if (!file.is_open())
            {
                std::cerr << "XMLUtils: Failed to open file: " << filePath << std::endl;
                return {};
            }

            std::stringstream buffer;
            buffer << file.rdbuf();  // Read entire file contents into buffer
            file.close();
            return ParseXML(buffer.str());
        }

        // ---------------------------------------------------------------------
        // Function: SaveXML
        // Purpose : Writes an XMLNode tree to a file in formatted XML.
        // Params  : filePath - Where to save the XML data.
        //           root - The root XMLNode to write.
        // ---------------------------------------------------------------------
        static void SaveXML(const std::string& filePath, const XMLNode& root)
        {
            std::ofstream file(filePath);
            if (!file.is_open())
            {
                std::cerr << "XMLUtils: Failed to open file for writing: " << filePath << std::endl;
                return;
            }

            WriteNode(file, root, 0);
            file.close();
        }

    private:

        // ---------------------------------------------------------------------
        // Function: ParseNode
        // Purpose : Recursive helper that reads XML from a stream one node at a
        //           time. Handles both text values and nested child tags.
        // Returns : The fully parsed XMLNode for the current level.
        // ---------------------------------------------------------------------
        static XMLNode ParseNode(std::istringstream& stream)
        {
            XMLNode node;
            std::string line;

            while (std::getline(stream, line))
            {
                // Remove all whitespace so tags are easier to detect
                line.erase(std::remove_if(line.begin(), line.end(), ::isspace), line.end());
                if (line.empty()) continue; // Skip blank lines

                // Stop if this is a closing tag (ex: </User>)
                if (line.find("</") != std::string::npos)
                    break;

                // If the line starts with an opening tag like <Name>
                if (line[0] == '<' && line[1] != '/')
                {
                    size_t closeTag = line.find('>');
                    if (closeTag == std::string::npos)
                        continue; // malformed tag, skip

                    // Extract tag name between < and >
                    node.name = line.substr(1, closeTag - 1);

                    // Check for a full tag on one line (ex: <Name>John</Name>)
                    size_t endTag = line.find("</" + node.name + ">");
                    if (endTag != std::string::npos)
                    {
                        // Extract inner value between opening and closing tags
                        node.value = line.substr(closeTag + 1, endTag - closeTag - 1);
                    }
                    else
                    {
                        // Otherwise, recursively parse child nodes
                        XMLNode child = ParseNode(stream);
                        node.children.push_back(child);
                    }
                }
            }

            return node;
        }

        // ---------------------------------------------------------------------
        // Function: WriteNode
        // Purpose : Recursively writes a node (and its children) to an XML file.
        // Params  : file  - Output stream to write to.
        //           node  - The current node being written.
        //           depth - Indentation level for pretty formatting.
        // ---------------------------------------------------------------------
        static void WriteNode(std::ofstream& file, const XMLNode& node, int depth)
        {
            std::string indent(depth * 2, ' '); // Indent for readability

            // Start opening tag
            file << indent << "<" << node.name << ">";

            if (!node.children.empty())
            {
                // If node has children, write them on new lines
                file << "\n";
                for (const auto& child : node.children)
                    WriteNode(file, child, depth + 1);
                file << indent; // Indent before closing tag
            }
            else if (!node.value.empty())
            {
                // If no children, write the text value inside the same line
                file << node.value;
            }

            // Write closing tag
            file << "</" << node.name << ">\n";
        }
    };
}

