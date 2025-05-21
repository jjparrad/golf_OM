#include <vector>
#include <stdio.h>

#include <iostream>
#include <cstring>
#include <fstream>

#include <glm/glm.hpp>
#include <map>
#include <sstream>




#include "common/objloader.hpp"

// Very, VERY simple OBJ loader.
// Here is a short list of features a real function would provide : 
// - Binary files. Reading a model should be just a few memcpy's away, not parsing a file at runtime. In short : OBJ is not very great.
// - Animations & bones (includes bones weights)
// - Multiple UVs
// - All attributes should be optional, not "forced"
// - More stable. Change a line in the OBJ file and it crashes.
// - More secure. Change another line and you can inject code.
// - Loading from memory, stream, etc

std::string loadMTL(const char* mtl_path) {
    std::ifstream mtlFile(mtl_path);
    if (!mtlFile) {
        std::cerr << "Impossible d'ouvrir le fichier MTL : " << mtl_path << std::endl;
        return "";
    }

    std::string line;
    std::string texturePath;
    while (std::getline(mtlFile, line)) {
        std::istringstream iss(line);
        std::string keyword;
        iss >> keyword;
        if (keyword == "map_Kd") {
            iss >> texturePath;
            break;
        }
    }

    return texturePath;
}

bool loadOBJ(const char* obj_path, const char* mtl_path,
             std::vector<unsigned short>& out_indices,
             std::vector<glm::vec3>& out_vertices,
             std::vector<glm::vec2>& out_uvs)
{
    std::ifstream objFile(obj_path);
    if (!objFile) {
        std::cerr << "Impossible d'ouvrir le fichier OBJ : " << obj_path << std::endl;
        return false;
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<std::pair<unsigned int, unsigned int>> index_map;
    std::map<std::pair<unsigned int, unsigned int>, unsigned short> unique_map;

    std::string line;
    while (std::getline(objFile, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        } else if (prefix == "vt") {
            glm::vec2 uv;
            iss >> uv.x >> uv.y;

            uv.y = 1.0 - uv.y;
            
            temp_uvs.push_back(uv);
        } else if (prefix == "f") {
            for (int i = 0; i < 3; i++) {
                std::string token;
                iss >> token;
                size_t pos1 = token.find('/');
                size_t pos2 = token.find('/', pos1 + 1);

                unsigned int vi = std::stoi(token.substr(0, pos1)) - 1;
                unsigned int ti = std::stoi(token.substr(pos1 + 1, pos2 - pos1 - 1)) - 1;

                std::pair<unsigned int, unsigned int> key = {vi, ti};

                if (unique_map.find(key) == unique_map.end()) {
                    out_vertices.push_back(temp_vertices[vi]);
                    out_uvs.push_back(temp_uvs[ti]);
                    unsigned short new_index = (unsigned short)(out_vertices.size() - 1);
                    unique_map[key] = new_index;
                    out_indices.push_back(new_index);
                } else {
                    out_indices.push_back(unique_map[key]);
                }
            }
        }
    }

    // Charger la texture depuis le MTL
    std::string texturePath = loadMTL(mtl_path);
    std::cout << "Texture chargée : " << texturePath << std::endl;

    return true;
}


bool loadOBJ(const char* obj_path,
             std::vector<unsigned short>& out_indices,
             std::vector<glm::vec3>& out_vertices,
             std::vector<glm::vec2>& out_uvs)
{
    std::ifstream objFile(obj_path);
    if (!objFile) {
        std::cerr << "Impossible d'ouvrir le fichier OBJ : " << obj_path << std::endl;
        return false;
    }

    std::vector<glm::vec3> temp_vertices;
    std::vector<glm::vec2> temp_uvs;
    std::vector<std::pair<unsigned int, unsigned int>> index_map;
    std::map<std::pair<unsigned int, unsigned int>, unsigned short> unique_map;

    std::string line;
    while (std::getline(objFile, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            temp_vertices.push_back(vertex);
        } else if (prefix == "vt") {
            glm::vec2 uv;
            iss >> uv.y >> uv.x;
            temp_uvs.push_back(uv);
        } else if (prefix == "f") {
            for (int i = 0; i < 3; i++) {
                std::string token;
                iss >> token;
                size_t pos1 = token.find('/');
                size_t pos2 = token.find('/', pos1 + 1);

                unsigned int vi = std::stoi(token.substr(0, pos1)) - 1;
                unsigned int ti = std::stoi(token.substr(pos1 + 1, pos2 - pos1 - 1)) - 1;

                std::pair<unsigned int, unsigned int> key = {vi, ti};

                if (unique_map.find(key) == unique_map.end()) {
                    out_vertices.push_back(temp_vertices[vi]);
                    out_uvs.push_back(temp_uvs[ti]);
                    unsigned short new_index = (unsigned short)(out_vertices.size() - 1);
                    unique_map[key] = new_index;
                    out_indices.push_back(new_index);
                } else {
                    out_indices.push_back(unique_map[key]);
                }
            }
        }
    }

    return true;
}





bool loadOFF( const std::string & filename ,
              std::vector< glm::vec3 > & vertices ,
              std::vector< unsigned short > & indices,
              std::vector< std::vector<unsigned short > > & triangles )
{
    bool convertToTriangles = true;
    bool randomize = false;

    std::ifstream myfile;
    myfile.open(filename.c_str());
    if (!myfile.is_open())
    {
        std::cout << filename << " cannot be opened" << std::endl;
        return false;
    }

    std::string magic_s;

    myfile >> magic_s;

    if( magic_s != "OFF" )
    {
        std::cout << magic_s << " != OFF :   We handle ONLY *.off files." << std::endl;
        myfile.close();
        return false;
    }

    int n_vertices , n_faces , dummy_int;
    myfile >> n_vertices >> n_faces >> dummy_int;

    vertices.resize(n_vertices);

    for( int v = 0 ; v < n_vertices ; ++v )
    {
        glm::vec3 vertex;
        myfile >> vertex.x >> vertex.y >> vertex.z;
        if( std::isnan(vertex.x) )
            vertex.x = 0.0;
        if( std::isnan(vertex.y) )
            vertex.y = 0.0;
        if( std::isnan(vertex.z) )
            vertex.z = 0.0;
        vertices[v] = vertex;
    }


    for( int f = 0 ; f < n_faces ; ++f )
    {
        int n_vertices_on_face;
        myfile >> n_vertices_on_face;
        if( n_vertices_on_face == 3 )
        {
            unsigned short _v1 , _v2 , _v3;
            std::vector< unsigned short > _v;
            myfile >> _v1 >> _v2 >> _v3;
            _v.push_back( _v1 );
            _v.push_back( _v2 );
            _v.push_back( _v3 );
            triangles.push_back( _v );
            indices.push_back( _v1 );
            indices.push_back( _v2 );
            indices.push_back( _v3 );

        }
        else if( n_vertices_on_face > 3 )
        {
            std::vector< unsigned short > vhandles;
            vhandles.resize(n_vertices_on_face);
            for( int i=0 ; i < n_vertices_on_face ; ++i )
                myfile >> vhandles[i];

            if( convertToTriangles )
            {
                unsigned short k=(randomize)?(rand()%vhandles.size()):0;
                for (unsigned short i=0;i<vhandles.size()-2;++i)
                {
                    std::vector< unsigned short > tri(3);
                    tri[0]=vhandles[(k+0)%vhandles.size()];
                    tri[1]=vhandles[(k+i+1)%vhandles.size()];
                    tri[2]=vhandles[(k+i+2)%vhandles.size()];
                    triangles.push_back(tri);
                    indices.push_back(tri[0]);
                    indices.push_back(tri[1]);
                    indices.push_back(tri[2]);
                }
            }
            else
            {
                //careful not triangles
                triangles.push_back(vhandles);
                for( int i=0 ; i < vhandles.size() ; ++i )
                    indices.push_back(vhandles[i]);
            }
        }
        else
        {
            std::cout << "OFFIO::open error : Face number " << f << " has " << n_vertices_on_face << " vertices" << std::endl;
            myfile.close();
            return false;
        }
    }

    myfile.close();
    return true;
}


bool loadOFF( const std::string & filename ,
              std::vector< glm::vec3 > & vertices,
              std::vector< unsigned short > & faces)
{
    bool convertToTriangles = true;
    bool randomize = false;

    std::ifstream myfile;
    myfile.open(filename.c_str());
    if (!myfile.is_open())
    {
        std::cout << filename << " cannot be opened" << std::endl;
        return false;
    }

    std::string magic_s;

    myfile >> magic_s;

    if( magic_s != "OFF" )
    {
        std::cout << magic_s << " != OFF :   We handle ONLY *.off files." << std::endl;
        myfile.close();
        return false;
    }

    int n_vertices , n_faces , dummy_int;
    myfile >> n_vertices >> n_faces >> dummy_int;

    vertices.resize(n_vertices);

    for( int v = 0 ; v < n_vertices ; ++v )
    {
        glm::vec3 vertex;
        myfile >> vertex.x >> vertex.y >> vertex.z;
        if( std::isnan(vertex.x) )
            vertex.x = 0.0;
        if( std::isnan(vertex.y) )
            vertex.y = 0.0;
        if( std::isnan(vertex.z) )
            vertex.z = 0.0;
        vertices[v] = vertex;
    }


    for( int f = 0 ; f < n_faces ; ++f )
    {
        int n_vertices_on_face;
        myfile >> n_vertices_on_face;
        if( n_vertices_on_face == 3 )
        {
            unsigned short _v1 , _v2 , _v3;
            std::vector< unsigned short > _v;
            myfile >> _v1 >> _v2 >> _v3;
            //            _v.push_back( _v1 );
            //            _v.push_back( _v2 );
            //            _v.push_back( _v3 );
            //            faces.push_back( _v );
            faces.push_back( _v1 );
            faces.push_back( _v2 );
            faces.push_back( _v3 );

        }
        else if( n_vertices_on_face > 3 )
        {
            std::vector< unsigned short > vhandles;
            vhandles.resize(n_vertices_on_face);
            for( int i=0 ; i < n_vertices_on_face ; ++i )
                myfile >> vhandles[i];

            if( convertToTriangles )
            {
                unsigned short k=(randomize)?(rand()%vhandles.size()):0;
                for (unsigned short i=0;i<vhandles.size()-2;++i)
                {
                    std::vector< unsigned short > tri(3);
                    tri[0]=vhandles[(k+0)%vhandles.size()];
                    tri[1]=vhandles[(k+i+1)%vhandles.size()];
                    tri[2]=vhandles[(k+i+2)%vhandles.size()];
                    //faces.push_back(tri);
                    faces.push_back(tri[0]);
                    faces.push_back(tri[1]);
                    faces.push_back(tri[2]);
                }
            }
            else
            {
                //faces.push_back(vhandles);
                for( int i=0 ; i < vhandles.size() ; ++i )
                    faces.push_back(vhandles[i]);
            }
        }
        else
        {
            std::cout << "OFFIO::open error : Face number " << f << " has " << n_vertices_on_face << " vertices" << std::endl;
            myfile.close();
            return false;
        }
    }

    myfile.close();
    return true;
}



#ifdef USE_ASSIMP // don't use this #define, it's only for me (it AssImp fails to compile on your machine, at least all the other tutorials still work)

// Include AssImp
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

bool loadAssImp(
        const char * path,
        std::vector<unsigned short> & indices,
        std::vector<glm::vec3> & vertices,
        std::vector<glm::vec2> & uvs,
        std::vector<glm::vec3> & normals
        ){

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(path, 0/*aiProcess_JoinIdenticalVertices | aiProcess_SortByPType*/);
    if( !scene) {
        fprintf( stderr, importer.GetErrorString());
        getchar();
        return false;
    }
    const aiMesh* mesh = scene->mMeshes[0]; // In this simple example code we always use the 1rst mesh (in OBJ files there is often only one anyway)

    // Fill vertices positions
    vertices.reserve(mesh->mNumVertices);
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
        aiVector3D pos = mesh->mVertices[i];
        vertices.push_back(glm::vec3(pos.x, pos.y, pos.z));
    }

    // Fill vertices texture coordinates
    uvs.reserve(mesh->mNumVertices);
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
        aiVector3D UVW = mesh->mTextureCoords[0][i]; // Assume only 1 set of UV coords; AssImp supports 8 UV sets.
        uvs.push_back(glm::vec2(UVW.x, UVW.y));
    }

    // Fill vertices normals
    normals.reserve(mesh->mNumVertices);
    for(unsigned int i=0; i<mesh->mNumVertices; i++){
        aiVector3D n = mesh->mNormals[i];
        normals.push_back(glm::vec3(n.x, n.y, n.z));
    }


    // Fill face indices
    indices.reserve(3*mesh->mNumFaces);
    for (unsigned int i=0; i<mesh->mNumFaces; i++){
        // Assume the model has only triangles.
        indices.push_back(mesh->mFaces[i].mIndices[0]);
        indices.push_back(mesh->mFaces[i].mIndices[1]);
        indices.push_back(mesh->mFaces[i].mIndices[2]);
    }

    // The "scene" pointer will be deleted automatically by "importer"
    return true;
}

#endif
