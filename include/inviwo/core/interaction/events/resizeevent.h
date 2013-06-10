#ifndef IVW_RESIZEEVENT_H
#define IVW_RESIZEEVENT_H

#include <inviwo/core/common/inviwocoredefine.h>
#include <inviwo/core/common/inviwo.h>
#include <inviwo/core/interaction/events/event.h>

namespace inviwo {

    class IVW_CORE_API ResizeEvent : public Event {

    public:        
        ResizeEvent(uvec2 newSize);
        ~ResizeEvent();
        inline uvec2 size() const {return size_; }
        inline uvec2 previousSize() const {return previousSize_; }
        inline void setSize(uvec2 csize) {size_ = csize;} 
        inline void setPreviousSize(uvec2 previousSize) {previousSize_ = previousSize;}     

    private:        
        uvec2 size_;
        uvec2 previousSize_;
    };

} // namespace

#endif // IVW_RESIZEEVENT_H