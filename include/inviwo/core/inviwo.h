#ifndef IVW_INVIWO_H
#define IVW_INVIWO_H

#define IVW_DEBUG //TODO: move into project file

#define IVW_DIR std::string("D:/inviwo/")

#pragma warning(disable: 4290)

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

// error handling
#include "inviwo/core/util/assertion.h"
#include "inviwo/core/util/exception.h"
#include "inviwo/core/util/logdistributor.h"
#define IVW_UNUSED_PARAM(param) (void)param

//#include "ext/voreen/serialization/serialization.h"
#include "inviwo/core/io/serialization/ivwserialization.h"

typedef signed char         INT8;
typedef signed short        INT16;
typedef signed int          INT32;
typedef signed __int64      INT64;
typedef unsigned char       UINT8;
typedef unsigned short      UINT16;
typedef unsigned int        UINT32;
typedef unsigned __int64    UINT64;

// include glm
#include "ext/glm/glm.hpp"
#include "ext/glm/gtx/transform.hpp"
#include "ext/glm/gtc/quaternion.hpp"
#include "ext/glm/gtx/quaternion.hpp"
#include "ext/glm/core/type.hpp"
#include "ext/glm/gtc/type_ptr.hpp"

typedef glm::ivec2 ivec2;
typedef glm::ivec3 ivec3;
typedef glm::ivec4 ivec4;
typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::dvec2 dvec2;
typedef glm::dvec3 dvec3;
typedef glm::dvec4 dvec4;
typedef glm::bvec2 bvec2;
typedef glm::bvec3 bvec3;
typedef glm::bvec3 bvec4;
typedef glm::mat2 mat2;
typedef glm::mat3 mat3;
typedef glm::mat4 mat4;
typedef glm::quat quat;

//using namespace voreen;

#endif // IVW_INVIWO_H
