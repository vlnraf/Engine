#pragma once

#ifdef KIT_EXPORT
#define KIT_API __declspec(dllexport)
#else
#define KIT_API __declspec(dllimport)
#endif