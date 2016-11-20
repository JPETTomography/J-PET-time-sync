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
	EXPECT_THROW(map.operator[]({.layer=0,.slot=0}),Exception<JPetMap<size_t>>);
	EXPECT_THROW(map.operator[]({.layer=0,.slot=1}),Exception<JPetMap<size_t>>);
	EXPECT_EQ(2,map.LayerSize(1));
	EXPECT_THROW(map.operator[]({.layer=1,.slot=0}),Exception<JPetMap<size_t>>);
	EXPECT_NO_THROW(map.operator[]({.layer=1,.slot=1}));
	EXPECT_NO_THROW(map.operator[]({.layer=1,.slot=2}));
	EXPECT_THROW(map.operator[]({.layer=1,.slot=3}),Exception<JPetMap<size_t>>);
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
TEST(JPetMap,Positioning){
    JPetMap<size_t> map({2,3,8});
    const auto initpos=map.PositionOfGlobalNumber(0);
    EXPECT_EQ(initpos.layer,1);
    EXPECT_EQ(initpos.slot,1);
    EXPECT_EQ(0,map.GlobalSlotNumber(initpos));
    for(size_t cur=1;cur<map.size();cur++){
	const size_t prev=cur-1;
	const auto prevpos=map.PositionOfGlobalNumber(prev);
	const auto curpos=map.PositionOfGlobalNumber(cur);
	EXPECT_EQ(cur,map.GlobalSlotNumber(curpos));
	if(prevpos.layer==curpos.layer){
	    EXPECT_EQ(prevpos.slot+1,curpos.slot);
	}else{
	    if((prevpos.layer+1)==curpos.layer){
		EXPECT_EQ(1,curpos.slot);
		EXPECT_EQ(map.LayerSize(prevpos.layer),prevpos.slot);
	    }else ASSERT_TRUE(false);
	}
    }
}

