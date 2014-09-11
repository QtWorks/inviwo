/*********************************************************************************
 *
 * Inviwo - Interactive Visualization Workshop
 * Version 0.6b
 *
 * Copyright (c) 2014 Inviwo Foundation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Main file authors: Erik Sund�n
 *
 *********************************************************************************/

#ifndef IVW_GESTUREEVENT_H
#define IVW_GESTUREEVENT_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/interaction/events/interactionevent.h>

namespace inviwo {

class IVW_CORE_API GestureEvent : public InteractionEvent {
public:
    enum GestureType {
        PAN   =      0,
        PINCH,
        SWIPE,
        COUNT
    };

    enum GestureState {
        GESTURE_STATE_NONE    =      0,
        GESTURE_STATE_STARTED,
        GESTURE_STATE_UPDATED,
        GESTURE_STATE_ENDED,
        GESTURE_STATE_CANCELED
    };

    GestureEvent(vec2 deltaPos, double deltaDistance, GestureEvent::GestureType type, GestureEvent::GestureState state, int numFingers, vec2 screenPosNorm);
    ~GestureEvent();

    inline vec2 deltaPos() const { return deltaPos_; }
    inline double deltaDistance() const { return deltaDistance_; }
    inline GestureEvent::GestureType type() const { return type_; }
    inline GestureEvent::GestureState state() const { return state_; }
    inline int numFingers() { return numFingers_; }
    inline vec2 screenPosNormalized() { return screenPosNorm_; }

    void modify(vec2);

    virtual std::string getClassIdentifier() const { return "GestureEvent"; }

    virtual void serialize(IvwSerializer& s) const;
    virtual void deserialize(IvwDeserializer& d);

private:
    vec2 deltaPos_;
    double deltaDistance_;
    GestureEvent::GestureType type_;
    GestureEvent::GestureState state_;
    int numFingers_;
    vec2 screenPosNorm_;
};

} // namespace

#endif // IVW_GESTUREEVENT_H