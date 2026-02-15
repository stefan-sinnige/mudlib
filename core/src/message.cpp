/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#include "mud/core/message.h"

BEGIN_MUDLIB_CORE_NS

/* =========================================================================
 * Public Interface
 * ========================================================================= */

/* static */
broker&
broker::instance()
{
    static broker sInstance;
    return sInstance;
}

void
broker::attach(const mud::core::uuid& topic, method&& meth)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    auto entry = _subscriptions.find(topic);
    if (entry == _subscriptions.end()) {
        auto result = _subscriptions.emplace(topic, methods());
        if (!result.second) {
            return;
        }
        entry = result.first;
    }
    auto found = std::find(entry->second.begin(),
                           entry->second.end(),
                           meth);
    if (found == entry->second.end()) {
        entry->second.emplace_back(std::move(meth));
    }
}

void
broker::detach(const mud::core::uuid& topic, method&& meth)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    auto entry = _subscriptions.find(topic);
    if (entry != _subscriptions.end()) {
        auto found = std::find(entry->second.begin(),
                               entry->second.end(),
                               meth);
        if (found != entry->second.end()) {
            entry->second.erase(found);
        }
    }
}

void
broker::detach(void* obj)
{
    if (obj == nullptr) {
        return;
    }
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    for (auto& subscription: _subscriptions) {
        auto method_it = subscription.second.begin();
        while (method_it != subscription.second.end()) {
            if (method_it->object() == obj) {
                method_it = subscription.second.erase(method_it);
            }
            else {
                ++method_it;
            }
        }
    }
}

void
broker::move(void* src, void* dst)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);
    for (auto& subscription: _subscriptions) {
        for (auto& method: subscription.second) {
            if (method.object() == src) {
                method.object(dst);
            }
        }
    }
}

size_t
broker::size(const mud::core::uuid& topic)
{
    std::lock_guard<std::recursive_mutex> lock(instance()._mutex);
    auto entry = instance()._subscriptions.find(topic);
    if (entry != instance()._subscriptions.end()) {
        return entry->second.size();
    }
    return 0;
}

void
broker::notify(const message& msg)
{
    std::lock_guard<std::recursive_mutex> lock(_mutex);

    /* Run all the subscriptions for this topic on a copy, such that any
     * subscription handler can modify the original subscription structure
     * as long it is all executed from the same thread. */
    auto entry = _subscriptions.find(msg.topic());
    if (entry != _subscriptions.end()) {
        auto copy = entry->second;
        for (auto& method: copy) {
            method(msg);
        }
    }
}

END_MUDLIB_CORE_NS
