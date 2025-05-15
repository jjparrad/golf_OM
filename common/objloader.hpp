#ifndef OBJLOADER_H
#define OBJLOADER_H
#include <string>


std::string loadMTL(const char* mtl_path);

bool loadOBJ(const char* obj_path, const char* mtl_path,
             std::vector<unsigned short>& out_indices,
             std::vector<glm::vec3>& out_vertices,
             std::vector<glm::vec2>& out_uvs);

bool loadOBJ(const char* obj_path,
             std::vector<unsigned short>& out_indices,
             std::vector<glm::vec3>& out_vertices,
             std::vector<glm::vec2>& out_uvs);

bool loadAssImp(
	const char * path, 
	std::vector<unsigned short> & indices,
	std::vector<glm::vec3> & vertices,
	std::vector<glm::vec2> & uvs,
	std::vector<glm::vec3> & normals
);


bool loadOFF( const std::string & filename ,
              std::vector< glm::vec3 > & vertices ,
              std::vector< unsigned short > & faces) ;


bool loadOFF( const std::string & filename ,
              std::vector< glm::vec3 > & vertices ,
              std::vector< unsigned short > & indices,
              std::vector< std::vector<unsigned short > > & triangles) ;
#endif
