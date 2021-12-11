#include "cocoa/cocoa_pushbutton.h"
#include "cocoa/cocoa_window.h"
#include "mud/ui/application.h"
#include "mud/ui/pushbutton.h"
#include "mud/ui/task.h"

@implementation PushButton
@end

BEGIN_MUDLIB_UI_NS

pushbutton::impl::impl(pushbutton& pb, window& parent)
  : cocoa::control(pb), _pushbutton(pb), _parent(parent), _btn(nullptr)
{
    window::impl::get(_parent)->add_control(pb);
}

pushbutton::impl::~impl() {}

void
pushbutton::impl::initialise()
{
    _btn = [[[PushButton alloc]
        initWithFrame:NSMakeRect(_pushbutton.property<position>().x(),
                                 _pushbutton.property<position>().y(),
                                 _pushbutton.property<size>().width(),
                                 _pushbutton.property<size>().height())]
        autorelease];
    if (!_btn) {
        throw std::runtime_error("cannot create pushbutton");
    }
    [_btn setButtonType:NSButtonTypeMomentaryPushIn];
    [_btn setTitle:[NSString stringWithUTF8String:_pushbutton.property<text>()
                                                      .value()
                                                      .c_str()]];
    [_btn setBezelStyle:NSBezelStyleRounded];

    Window* wnd = window::impl::get(_parent)->NativeWindow();
    [[wnd contentView] addSubview:_btn];
}

void
pushbutton::impl_deleter::operator()(pushbutton::impl* ptr) const
{
    delete ptr;
}

/** The explicit specialisation of a UI pushbutton */

pushbutton::pushbutton(window& parent)
{
    default_properties();
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(*this, parent));
}

pushbutton::~pushbutton() {}

void
pushbutton::initialise()
{
    _impl->initialise();
}

void
pushbutton::dispatch(const mud::ui::event& event)
{
    switch (event.type()) {
        case mud::ui::event::type_t::MOUSE:
            if (_mouse_event_fn != nullptr) {
                auto& ev = static_cast<const mud::ui::event::mouse&>(event);
                _mouse_event_fn(ev);
            }
            break;
        default:
            /* Not handled */
            break;
    }
}

END_MUDLIB_UI_NS

/* vi: set ai ts=4 expandtab: */
