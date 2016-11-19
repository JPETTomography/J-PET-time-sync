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
TEST(JPetMap, sizes){
    {
	JPetMap<size_t> map({2});
	EXPECT_THROW(map.LayerSize(0),Exception<JPetMap<size_t>>);
	EXPECT_EQ(2,map.LayerSize(1));
	EXPECT_THROW(map.LayerSize(2),Exception<JPetMap<size_t>>);
    }{
	JPetMap<size_t> map({2,3});
	EXPECT_THROW(map.LayerSize(0),Exception<JPetMap<size_t>>);
	EXPECT_THROW(map.item({.layer=0,.slot=0}),Exception<JPetMap<size_t>>);
	EXPECT_THROW(map.item({.layer=0,.slot=1}),Exception<JPetMap<size_t>>);
	EXPECT_EQ(2,map.LayerSize(1));
	EXPECT_THROW(map.item({.layer=1,.slot=0}),Exception<JPetMap<size_t>>);
	EXPECT_NO_THROW(map.item({.layer=1,.slot=1}));
	EXPECT_NO_THROW(map.item({.layer=1,.slot=2}));
	EXPECT_THROW(map.item({.layer=1,.slot=3}),Exception<JPetMap<size_t>>);
	EXPECT_EQ(3,map.LayerSize(2));
	EXPECT_THROW(map.LayerSize(3),Exception<JPetMap<size_t>>);
    }{
	JPetMap<size_t> map({2,3,8});
	EXPECT_THROW(map.LayerSize(0),Exception<JPetMap<size_t>>);
	EXPECT_EQ(2,map.LayerSize(1));
	EXPECT_EQ(3,map.LayerSize(2));
	EXPECT_EQ(8,map.LayerSize(3));
	EXPECT_THROW(map.LayerSize(4),Exception<JPetMap<size_t>>);
    }
}
