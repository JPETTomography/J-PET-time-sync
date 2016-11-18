// this file is distributed under 
// MIT license
#include <random>
#include <gtest/gtest.h>
#include <math_h/error.h>
#include <j-pet-framework-extension/PetDict.h>
using namespace std;
using namespace MathTemplates;
TEST(JPetMap,empty){
    JPetMap<size_t> map({});
    EXPECT_EQ(0,map.LayersCount());
    EXPECT_THROW(map.LayerSize(0),Exception<JPetMap<size_t>>);
    EXPECT_THROW(map.LayerSize(1),Exception<JPetMap<size_t>>);
    EXPECT_THROW(map.LayerSize(2),Exception<JPetMap<size_t>>);
}
