#include <stdio.h>
#include <stdint.h>
#include "upng.h"
#include "texture.h"

tex2_t tex2_clone(tex2_t *t)
{
    return (tex2_t){t->u, t->v};
};
