#include "readObjFile.hpp"

#include <iostream>
#include <array>
#include <map>

#include "tiny_obj_loader.hpp"
#include "stb_image.h"

using namespace std;

using VertIdxMap = map<tinyobj::index_t, GLuint>;

static inline
vector<BAOelem> buildBAOfromIdxVertMap(const vector<VertIdxMap::iterator>& idxVertMap, const tinyobj::attrib_t& attr) {

    auto& vertices = attr.vertices;
    auto& normals = attr.normals;
    auto& texCoords = attr.texcoords;
    
    vector<BAOelem> bao;
    bao.resize(idxVertMap.size());
    for (auto i : idxVertMap) {
        auto& vertInfos = i->first;
        bao[i->second] = { &vertices[3 * vertInfos.vertex_index], &normals[3 * vertInfos.normal_index], &texCoords[2 * vertInfos.texcoord_index] };
    }

    return bao;
}

//bao, objects<(eao, materialID)>, materials tuple
tuple<vector<BAOelem>, vector<pair<vector<GLuint>, size_t>>, vector<string>> readObjFile(const std::string& objFilePath) {

    tinyobj::ObjReader objReader;

    if (!objReader.ParseFromFile(objFilePath)) {
        std::cerr << objReader.Error() << std::endl;
        return {};
    }

    if (objReader.Warning().size())
        std::cerr << objReader.Warning() << std::endl;

    auto& shapes = objReader.GetShapes();
    VertIdxMap vertIdxMap;
    vector<VertIdxMap::iterator> idxVertMap;
    vector<pair<vector<GLuint>, size_t>> groups;
    GLuint idxCounter = 0;
    //new vao per material


    for (auto& shape : shapes) {
        groups.emplace_back(vector<GLuint>{}, -1);

        auto& mesh = shape.mesh;
        auto& mtlIdxs = mesh.material_ids;
        idxVertMap.reserve(idxVertMap.size() + mesh.indices.size());


        for (size_t i = 0; i < mesh.indices.size(); i++) {
            auto& vert = shape.mesh.indices[i];
            auto pos = vertIdxMap.lower_bound(vert);
        
            if (pos == vertIdxMap.end() || pos->first != vert) {
                pos = vertIdxMap.insert(pos, VertIdxMap::value_type{ vert, idxCounter++ });
                idxVertMap.emplace_back(pos);
            }
            //check if all obj files and each group has material
            if (groups.back().second == -1) 
                groups.back().second = mtlIdxs[i / 3];
            else if (groups.back().second != mtlIdxs[i / 3]) {
                groups.emplace_back(vector<GLuint>{}, mtlIdxs[i / 3]);
            }

            groups.back().first.emplace_back(pos->second);
        }
    }

    vector<string> mtlFileNames;
    mtlFileNames.reserve(shapes.size());
    for (auto& mtls : objReader.GetMaterials()) {
        mtlFileNames.push_back(mtls.diffuse_texname);
    }
    
    return tuple<vector<BAOelem>, vector<pair<vector<GLuint>, size_t>>, vector<string>>(
        buildBAOfromIdxVertMap(idxVertMap, objReader.GetAttrib()),
        std::move(groups),
        std::move(mtlFileNames)
    );
}