// Copyright (C) 2024 owoDra

#pragma once

#include "Logging/LogMacros.h"

GEEXT_API DECLARE_LOG_CATEGORY_EXTERN(LogGEE, Log, All);

#if !UE_BUILD_SHIPPING

#define GEELOG(FormattedText, ...) UE_LOG(LogGEE, Log, FormattedText, __VA_ARGS__)

#define GEEENSURE(InExpression) ensure(InExpression)
#define GEEENSURE_MSG(InExpression, InFormat, ...) ensureMsgf(InExpression, InFormat, __VA_ARGS__)
#define GEEENSURE_ALWAYS_MSG(InExpression, InFormat, ...) ensureAlwaysMsgf(InExpression, InFormat, __VA_ARGS__)

#define GEECHECK(InExpression) check(InExpression)
#define GEECHECK_MSG(InExpression, InFormat, ...) checkf(InExpression, InFormat, __VA_ARGS__)

#else

#define GEELOG(FormattedText, ...)

#define GEEENSURE(InExpression) InExpression
#define GEEENSURE_MSG(InExpression, InFormat, ...) InExpression
#define GEEENSURE_ALWAYS_MSG(InExpression, InFormat, ...) InExpression

#define GEECHECK(InExpression) InExpression
#define GEECHECK_MSG(InExpression, InFormat, ...) InExpression

#endif