#pragma once

#include "esphome.h"

// Используем пространства имен ESPHome для сокращения кода
using namespace esphome;

// Структура для хранения координат и данных датчика
struct SensorBox {
    int x;
    int y;
    const char* label;
    float value;
};

/**
 * Отрисовка четырех боксов по углам дисплея
 * @param it Ссылка на объект дисплея
 * @param font Шрифт
 * @param values Массив из 4-х значений для отображения
 */
/**
 * Отрисовка верхней панели статуса
 * @param it Ссылка на дисплей
 * @param font Шрифт для букв
 * @param auto_backlight Включена ли автоподсветка
 * @param invert Включена ли инверсия
 * @param endstop_active Нажат ли концевик
 * @param night_return Включен ли автовозврат
 */
void draw_status_header(display::Display &it, display::BaseFont *font, bool auto_backlight, bool invert, bool endstop_active, bool night_return) {
    int center_x = it.get_width() / 2;
    int screen_w = it.get_width();
    int y = 0; // Сдвинуто вверх на 4 пикселя (было 4)

    // Центрируем группу букв
    // B - Backlight, N - Night return, I - Invert
    if (auto_backlight) it.print(center_x - 15, y, font, "B");
    if (night_return)   it.print(center_x - 2, y, font, "R");
    if (invert)         it.print(center_x + 11, y, font, "I");

    // Индикация эндстопа в углу
    if (endstop_active) {
        it.print(screen_w - 38, y, font, "E");
    }
}

/**
 * Отрисовка статуса трекера (угол и стрелки направления)
 * @param it Ссылка на дисплей
 * @param font Шрифт текста
 * @param angle Текущий угол в градусах
 * @param move_dir Направление движения: 0 - стоп, 1 - влево, 2 - вправо
 */
void draw_tracker_status(display::Display &it, display::BaseFont *font, float angle, int move_dir) {
    const char* left_arrow = (move_dir == 1) ? "<< " : "";
    const char* right_arrow = (move_dir == 2) ? " >>" : "";
    
    // Форматируем строку: << A:угол° >>
    // Y=20 (под буквами статуса)
    it.printf(64, 20, font, display::TextAlign::CENTER, "%sA:%.1f°%s", left_arrow, angle, right_arrow);
}

void draw_sensor_boxes(display::Display &it, display::BaseFont *font, const float *values, int box_w = 28, int box_h = 14) {
    int screen_w = it.get_width();
    int screen_h = it.get_height();

    // Массив координат и меток
    struct BoxConfig {
        int x;
        int y;
        const char* label;
    };

    BoxConfig boxes[] = {
        {-1, -1, "S1"},
        {(screen_w - box_w)+1, -1, "S2"},
        {-1, (screen_h - box_h)+1, "S3"},
        {(screen_w - box_w)+1, (screen_h - box_h)+1, "S4"}
    };

    for (int i = 0; i < 4; i++) {
        const auto& box = boxes[i];
        float value = values[i];

        // Отрисовка рамки
        it.rectangle(box.x, box.y, box_w, box_h);
        
        // Скругляем углы
        it.draw_pixel_at(box.x, box.y, display::COLOR_OFF);
        it.draw_pixel_at(box.x + box_w - 1, box.y, display::COLOR_OFF);
        it.draw_pixel_at(box.x, box.y + box_h - 1, display::COLOR_OFF);
        it.draw_pixel_at(box.x + box_w - 1, box.y + box_h - 1, display::COLOR_OFF);

        // Вывод значения
        if (std::isnan(value)) {
            it.printf(box.x + box_w / 2, box.y + box_h / 2, font, display::TextAlign::CENTER, "---");
        } else {
            it.printf(box.x + box_w / 2, box.y + box_h / 2, font, display::TextAlign::CENTER, "%.2f", value);
        }
    }
}
