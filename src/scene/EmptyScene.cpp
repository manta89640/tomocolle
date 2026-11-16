#include "EmptyScene.h"

#include "SceneMemory.h"

void EmptyScene::StartLoading() {
}

void EmptyScene::Init() {
}

void EmptyScene::Update() {
}

void EmptyScene::Draw() {
}

void EmptyScene::Exit() {
}

SceneMemory gSceneMemory;

// 0x70 bytes that overlays use (overlay 72 as a table of colours); this file defines them, after gSceneMemory
u8 gSceneScratch[0x70];
