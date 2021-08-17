#pragma once

#include <Windows.h>

constexpr float EDITOR_KEYFRAME_SIZE = 12.0f;
constexpr float EDITOR_BIN_HEIGHT = 20.0f;
constexpr float EDITOR_PLAY_BUTTON_SIZE = 20.0f;
constexpr int EDITOR_TIMELINE_BIN_COUNT = 15;
constexpr int EDITOR_PROP_BIN_COUNT = 5; 
constexpr float EDITOR_LABEL_AREA_WIDTH = 85.0f;
constexpr float EDITOR_KEYFRAME_OFFSET = 0.0f;
constexpr float EDITOR_TIME_POINTER_WIDTH = 18.0f;
constexpr float EDITOR_TIME_POINTER_HEIGHT = 30.0f;
constexpr float EDITOR_TIME_POINTER_TRI_HEIGHT = 8.0f;
constexpr float EDITOR_STRIP_LEFT = 8.0f;
constexpr float EDITOR_STRIP_RIGHT = 8.0f;
constexpr float EDITOR_STRIP_TEXT_LEFT_MARGIN = 4.0f;
constexpr float EDITOR_PROP_LABEL_TEXT_LEFT_MARGIN = 8.0f;

constexpr uint32_t EDITOR_PLAY_BUTTON_COL = 0xFFBABABA;
constexpr uint32_t EDITOR_BIN_PRIMARY_COL = 0xFF1F1F1F;
constexpr uint32_t EDITOR_BIN_SECONDARY_COL = 0xFF2E2E2E;
constexpr uint32_t EDITOR_KEYFRAME_INACTIVE_COL = 0xFFBABABA;
constexpr uint32_t EDITOR_KEYFRAME_ACTIVE_COL = 0xFF626262;
constexpr uint32_t EDITOR_STRIP_INACTIVE_COL = 0xFFBABABA;
constexpr uint32_t EDITOR_STRIP_ACTIVE_COL = 0xFF626262;

const uint32_t EDITOR_FORMAT_OBJECT = RegisterClipboardFormatA("NotJSBEditorObject");