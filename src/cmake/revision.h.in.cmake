#ifndef __REVISION_H__
#define __REVISION_H__
 #define _REVISION                  "@rev_id_str@"
 #define _HASH                      "@rev_hash@"
 #define _DATE                      "@rev_date@"
 #define _BRANCH                    "@rev_branch@"
 #define _CMAKE_COMMAND             R"(@CMAKE_COMMAND@)"
 #define _CMAKE_VERSION             R"(@CMAKE_VERSION@)"
 #define _CMAKE_HOST_SYSTEM         R"(@CMAKE_HOST_SYSTEM_NAME@ @CMAKE_HOST_SYSTEM_VERSION@)"
 #define _SOURCE_DIRECTORY          R"(@CMAKE_SOURCE_DIR@)"
 #define _BUILD_DIRECTORY           R"(@BUILDDIR@)"
 #define _MYSQL_EXECUTABLE          R"(@MYSQL_EXECUTABLE@)"
 #ifndef VER_COMPANYNAME_STR
  #define VER_COMPANYNAME_STR        "AzerothCore"
 #endif
 #ifndef VER_LEGALCOPYRIGHT_STR
  #define VER_LEGALCOPYRIGHT_STR     "(c)2016-@rev_year@ AzerothCore"
 #endif
 #ifndef VER_FILEVERSION
  #define VER_FILEVERSION            0,0,0
 #endif
 #ifndef VER_FILEVERSION_STR
  #define VER_FILEVERSION_STR        "@rev_hash@ @rev_date@ (@rev_branch@ branch)"
 #endif
 #ifndef VER_PRODUCTVERSION
  #define VER_PRODUCTVERSION         VER_FILEVERSION
 #endif
 #ifndef VER_PRODUCTVERSION_STR
  #define VER_PRODUCTVERSION_STR     VER_FILEVERSION_STR
 #endif
#endif // __REVISION_H__
