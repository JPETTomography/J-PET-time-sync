/**
 *  @copyright Copyright 2016 The J-PET Framework Authors. All rights reserved.
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may find a copy of the License in the LICENCE file.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *  @file TaskD.h
 */

#ifndef TASKD_H 
#define TASKD_H 

#include <JPetTask/JPetTask.h>
#include <JPetHit/JPetHit.h>
#include <JPetRawSignal/JPetRawSignal.h>
#include <IO/PetDict.h>
#include <Calc/convention.h>
#include "LargeBarrelMapping.h"

class JPetWriter;
class TOFCut:public JPetTask {
public:
  TOFCut(const char * name, const char * description,const std::shared_ptr<JPetMap<TOT_cut>>map);
  virtual ~TOFCut();
  virtual void init(const JPetTaskInterface::Options& opts)override;
  virtual void exec()override;
  virtual void terminate()override;
  virtual void setWriter(JPetWriter* writer)override;
protected:
  JPetWriter* fWriter;
  LargeBarrelMapping fBarrelMap;
  std::shared_ptr<JPetMap<TOT_cut>>f_map;
};
#endif