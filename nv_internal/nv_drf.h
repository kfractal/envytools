// -*- mode: c++; tab-width: 4; indent-tabs-mode: t; -*-
#pragma once
//
// Copyright (c) 2015, NVIDIA CORPORATION.  All rights reserved.
// insert GPL v2
//

#define DRF_BASE(drf)           (0?drf)
#define DRF_EXTENT(drf)         (1?drf)
#define DRF_SHIFT(drf)          ((0?drf) % 32)
#define DRF_MASK(drf)           (0xFFFFFFFF>>(31-((1?drf) % 32)+((0?drf) % 32)))
#define DRF_SHIFTMASK(drf)      (DRF_MASK(drf)<<(DRF_SHIFT(drf)))
#define DRF_SIZE(drf)           (DRF_EXTENT(drf)-DRF_BASE(drf)+1)

#define DRF_DEF(d,r,f,c)        ((NV ## d ## r ## f ## c)<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_NUM(d,r,f,n)        (((n)&DRF_MASK(NV ## d ## r ## f))<<DRF_SHIFT(NV ## d ## r ## f))
#define DRF_VAL(d,r,f,v)        (((v)>>DRF_SHIFT(NV ## d ## r ## f))&DRF_MASK(NV ## d ## r ## f))

#define NV_FIELD_LOWBIT(x)      (0?x)
#define NV_FIELD_HIGHBIT(x)     (1?x)
#define NV_FIELD_SIZE(x)        (NV_FIELD_HIGHBIT(x)-NV_FIELD_LOWBIT(x)+1)
#define NV_FIELD_SHIFT(x)       ((0?x)%32)
#define NV_FIELD_MASK(x)        (0xFFFFFFFFUL>>(31-((1?x)%32)+((0?x)%32)))
#define NV_FIELD_BITS(val, x)   (((val) & NV_FIELD_MASK(x))<<NV_FIELD_SHIFT(x))
#define NV_FIELD_SHIFTMASK(x)   (NV_FIELD_MASK(x)<< (NV_FIELD_SHIFT(x)))

#define NV_FIELD64_SHIFT(x) ((0?x)%64)
#define NV_FIELD64_MASK(x)  (0xFFFFFFFFFFFFFFFFULL>>(63-((1?x)%64)+((0?x)%64)))
#define NV_DRF_DEF(d,r,f,c) \
    ((d##_##r##_0_##f##_##c) << NV_FIELD_SHIFT(d##_##r##_0_##f##_RANGE))
#define NV_DRF_NUM(d,r,f,n) \
    (((n)& NV_FIELD_MASK(d##_##r##_0_##f##_RANGE)) << \
        NV_FIELD_SHIFT(d##_##r##_0_##f##_RANGE))
#define NV_DRF_VAL(d,r,f,v) \
    (((v)>> NV_FIELD_SHIFT(d##_##r##_0_##f##_RANGE)) & \
        NV_FIELD_MASK(d##_##r##_0_##f##_RANGE))
#define NV_FLD_SET_DRF_NUM(d,r,f,n,v) \
    ((v & ~NV_FIELD_SHIFTMASK(d##_##r##_0_##f##_RANGE)) | NV_DRF_NUM(d,r,f,n))
#define NV_FLD_SET_DRF_DEF(d,r,f,c,v) \
    (((v) & ~NV_FIELD_SHIFTMASK(d##_##r##_0_##f##_RANGE)) | \
        NV_DRF_DEF(d,r,f,c))
#define NV_RESETVAL(d,r)    (d##_##r##_0_RESET_VAL)
#define NV_DRF64_NUM(d,r,f,n) \
    (((n)& NV_FIELD64_MASK(d##_##r##_0_##f##_RANGE)) << \
        NV_FIELD64_SHIFT(d##_##r##_0_##f##_RANGE))
