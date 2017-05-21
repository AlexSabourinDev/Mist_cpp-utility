#pragma once

#define MIST_USE_NAMESPACE 1

#if MIST_USE_NAMESPACE
#define MIST_NAMESPACE \
namespace Mist \
{

#define MIST_NAMESPACE_END \
}
#else
#define MIST_NAMESPACE
#define MIST_NAMESPACE_END
#endif