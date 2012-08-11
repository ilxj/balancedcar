#pragma once

/*
 * These are exported to the viwer.
 */

b2Body * emulation_get_car();
void simulate_move_extra_load(b2Vec2 centor = b2Vec2(0,3));

void simulate_pause();
void simulate_resume();
