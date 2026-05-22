#pragma once

#include "esphome.h"

// Используем пространства имен ESPHome
using namespace esphome;

/**
 * Основная логика трекера.
 */
void update_tracker() {
    // Если калибровка (homing) не завершена, ничего не делаем
    if (!id(is_homed)) {
        return;
    }

    // Получаем текущие значения сенсоров и применяем калибровку
    float left = id(photoresistor_left).state + id(left_offset_val).state;
    float right = id(photoresistor_right).state + id(right_offset_val).state;

    // Если данные невалидны (NaN), ничего не делаем
    if (std::isnan(left) || std::isnan(right)) {
        return;
    }

    // Разница между датчиками
    float delta = left - right;
    
    // Порог срабатывания из интерфейса
    float threshold = id(tracking_threshold_val).state; 
    
    // Шаг перемещения
    int move_steps = 15;
    
    // Расчет лимитов
    float steps_per_deg = id(steps_per_degree_val).state;
    float max_steps = id(max_angle_val).state * steps_per_deg;
    int current_pos = id(tracker_stepper).current_position;

    if (std::abs(delta) > threshold) {
        // ИНВЕРТИРОВАННАЯ ЛОГИКА (сенсоры): 
        // Если на левом датчике напряжение МЕНЬШЕ (delta < 0), значит там СВЕТЛЕЕ.
        int target_delta = (delta < 0) ? move_steps : -move_steps;
        int target_pos = current_pos + target_delta;

        // Проверка программных лимитов (0 - max_steps)
        if (target_pos < 0) target_pos = 0;
        if (target_pos > (int)max_steps) target_pos = (int)max_steps;

        if (target_pos != current_pos) {
            id(tracker_stepper).set_target(target_pos);
            ESP_LOGD("tracker", "Move: Pos=%d, Target=%d, Delta=%.2f", current_pos, target_pos, delta);
        }
    }
}

/**
 * Возвращает текущее направление движения для дисплея
 * 0 - стоп, 1 - влево (к 0), 2 - вправо (к max)
 */
int get_move_direction() {
    int current = id(tracker_stepper).current_position;
    int target = id(tracker_stepper).target_position;
    
    if (target < current) return 1;
    if (target > current) return 2;
    return 0;
}

/**
 * Возвращает текущий угол в градусах
 */
float get_current_angle() {
    float steps_per_deg = id(steps_per_degree_val).state;
    if (steps_per_deg <= 0) return 0;
    return (float)id(tracker_stepper).current_position / steps_per_deg;
}
