/*
    Copyright (C) 2009 Andrew Caudwell (acaudwell@gmail.com)

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation; either version
    3 of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "action.h"

RAction::RAction(RUser* source, RFile* target, float addedtime) {
    this->source    = source;
    this->target    = target;
    this->addedtime = addedtime;

    progress = 0.0;

    rate = 0.5;
}

bool RAction::isFinished() {
    return (progress >= 1.0) ? true : false;
}

void RAction::logic(float dt) {
    if(progress >= 1.0) return;

    if(progress == 0.0) {
        target->touch(colour);
    }

    float action_rate = std::min(10.0f, rate * std::max(1.0f, ((float)source->getPendingActionCount())));

    progress = std::min(progress + action_rate * dt, 1.0f);
}

void RAction::draw(float dt) {
    if(isFinished()) return;

    glDisable(GL_TEXTURE_2D);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    vec2f src  = source->getPos();
    vec2f dest = target->getAbsolutePos();

    vec2f offset     = (dest - src).normal().perpendicular() * target->getSize() * 0.5;
    vec2f offset_src = offset * 0.3f;

    float max_alpha = 1.0;
    float alpha = max_alpha * (1.0 - progress);

    float alpha2 = alpha * 0.1;

    vec4f col1 = vec4f(colour, alpha);
    vec4f col2 = vec4f(colour, alpha2);

    glColor4f(1.0, 1.0, 1.0, 1.0);

    glBegin(GL_QUADS);
        glColor4fv(col2);

        glVertex2f(src.x - offset_src.x, src.y - offset_src.y);
        glVertex2f(src.x + offset_src.x, src.y + offset_src.y);

        glColor4fv(col1);

        glVertex2f(dest.x + offset.x, dest.y + offset.y);
        glVertex2f(dest.x - offset.x, dest.y - offset.y);
    glEnd();
}

CreateAction::CreateAction(RUser* source, RFile* target, float addedtime) : RAction(source, target, addedtime) {
    colour = vec3f(0.0, 1.0, 0.0);
    action = "Create";
}

RemoveAction::RemoveAction(RUser* source, RFile* target, float addedtime): RAction(source, target, addedtime) {
    colour = vec3f(1.0, 0.0, 0.0);
    action = "Remove";
}

void RemoveAction::logic(float dt) {
    float old_progress = progress;

    RAction::logic(dt);

    if(old_progress < 1.0 && progress >= 1.0) {
        target->remove();
    }
}

ModifyAction::ModifyAction(RUser* source, RFile* target, float addedtime) : RAction(source, target, addedtime) {
    colour = vec3f(1.0, 0.7, 0.3);
    action = "Modify";
}
