/**
 * bt_repeat.cpp
 * =============================================================================
 * Copyright 2021-2023 Serhii Snitsaruk
 *
 * Use of this source code is governed by an MIT-style
 * license that can be found in the LICENSE file or at
 * https://opensource.org/licenses/MIT.
 * =============================================================================
 */

#include "bt_repeat.h"

#include "core/object/object.h"
#include "core/string/ustring.h"
#include "core/variant/variant.h"

static String repeat_forever_str;

String BTRepeat::_generate_name() const {
	if (forever) {
		if (repeat_forever_str.is_empty()) {
			repeat_forever_str.parse_utf8("Repeat ∞");
		}
		return repeat_forever_str;
	}
	return vformat("Repeat x%s", times);
}

void BTRepeat::_enter() {
	cur_iteration = 1;
}

int BTRepeat::_tick(double p_delta) {
	ERR_FAIL_COND_V_MSG(get_child_count() == 0, FAILURE, "BT decorator has no child.");
	int status = get_child(0)->execute(p_delta);
	if (status == RUNNING || forever) {
		return RUNNING;
	} else if (status == FAILURE && abort_on_failure) {
		return FAILURE;
	} else if (cur_iteration >= times) {
		return SUCCESS;
	} else {
		cur_iteration += 1;
		return RUNNING;
	}
}

void BTRepeat::set_forever(bool p_forever) {
	forever = p_forever;
	notify_property_list_changed();
	emit_changed();
}

void BTRepeat::set_times(int p_value) {
	times = p_value;
	emit_changed();
}

void BTRepeat::set_abort_on_failure(bool p_value) {
	abort_on_failure = p_value;
	emit_changed();
}

void BTRepeat::_get_property_list(List<PropertyInfo> *p_list) const {
	if (!forever) {
		p_list->push_back(PropertyInfo(Variant::INT, "times", PROPERTY_HINT_RANGE, "1,65535"));
		p_list->push_back(PropertyInfo(Variant::BOOL, "abort_on_failure"));
	}
}

void BTRepeat::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_forever", "p_value"), &BTRepeat::set_forever);
	ClassDB::bind_method(D_METHOD("get_forever"), &BTRepeat::get_forever);
	ClassDB::bind_method(D_METHOD("set_times", "p_value"), &BTRepeat::set_times);
	ClassDB::bind_method(D_METHOD("get_times"), &BTRepeat::get_times);
	ClassDB::bind_method(D_METHOD("set_abort_on_failure", "p_value"), &BTRepeat::set_abort_on_failure);
	ClassDB::bind_method(D_METHOD("get_abort_on_failure"), &BTRepeat::get_abort_on_failure);

	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "forever"), "set_forever", "get_forever");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "times", PROPERTY_HINT_RANGE, "1,65535", PROPERTY_USAGE_NONE), "set_times", "get_times");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "abort_on_failure", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_abort_on_failure", "get_abort_on_failure");
}
