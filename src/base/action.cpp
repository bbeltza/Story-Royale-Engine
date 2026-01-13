#include <Base/Action.hpp>

using namespace sre;

Action* Action::head_ptr = NULL;

ConnectionHandle Action::sc_event;

Action::Action(): m_next(head_ptr)
{
	if (!sc_event.Connected())
		sc_event = sre::onEvent.Connect(sc_eventhandler, NULL);

	head_ptr = this;
	if (m_next) m_next->m_prev = this;
}

Action::~Action()
{
	if (m_prev)
		m_prev->m_next = m_next;
	else
	{
		assert(head_ptr == this);
		head_ptr = m_next;
	}
	if (m_next)
		m_next->m_prev = m_prev;
}

void Action::impl_getshiftmask(int category, int& shift, int& mask)
{
	switch (category)
	{
	case C_MOUSE:
		shift = C_MOUSE_SHIFT;
		mask = C_MOUSE_MASK;
		break;
	case C_SCANCODE:
		shift = C_SCANCODE_SHIFT;
		mask = C_SCANCODE_MASK;
		break;
	case C_KEYCODE:
		shift = C_KEYCODE_SHIFT;
		mask = C_KEYCODE_MASK;
		break;
	default:
		abort();
	}
}

void Action::add_impl(int value, int category)
{
	int shift;
	int mask;
	impl_getshiftmask(category, shift, mask);
	value &= mask;

	unsigned const ored = value << shift;
	for (unsigned& val : m_inputs)
	{
		unsigned const shifted = (val >> shift) & mask;
		if (shifted == value) return;
		if (shifted) continue;

		val |= ored;
		return;
	}

	m_inputs.push_back(0);
	m_inputs.back() |= ored;
}

void Action::remove_impl(int value, int category)
{
	int shift;
	int mask;
	impl_getshiftmask(category, shift, mask);
	value &= mask;

	for (unsigned& val : m_inputs)
	{
		unsigned const shifted = (val >> shift) & mask;
		if (shifted != value) continue;

		val &= ~mask;
		break;
	}
}



void Action::sc_eventhandler(void*, void*, Event ev)
{
	switch (ev.type)
	{
	case EVENT_KEYPRESS:
		if (ev.key_press.press == 2) return;
		{
			const int scancode_val = ev.key_press.scancode & C_SCANCODE_MASK;
			const int keycode_val = ev.key_press.keycode & C_KEYCODE_MASK;
			const int incrementer = ev.key_press.press ? 1 : -1;

			auto current = head_ptr;
			while (current)
			{
				bool found = false;
				for (unsigned val : current->m_inputs)
				{
					unsigned const shifted_scancode = (val >> C_SCANCODE_SHIFT) & C_SCANCODE_MASK;
					unsigned const shifted_keycode = (val >> C_KEYCODE_SHIFT) & C_KEYCODE_MASK;
					if (shifted_scancode != scancode_val && shifted_keycode != keycode_val) continue;

					current->m_counter += incrementer;
					found = true;
				}

				if (!found) goto END;

				if (!current->m_counter)
					current->m_frame = -1;
				else if (ev.key_press.press)
					current->m_frame = static_cast<int>(sre::current_frame());

			END:
				current = current->m_next;
			}
		}

		break;
	case EVENT_MOUSEBUTTON:
	{
		int incrementer = ev.mouse_button.pressed ? 1 : -1;

		auto current = head_ptr;
		while (current)
		{
			for (unsigned val : current->m_inputs)
			{
				if ((val & C_MOUSE_MASK) != ev.mouse_button.button) continue;

				current->m_counter += incrementer;
				if (!current->m_counter)
					current->m_frame = -1;
				else if (ev.mouse_button.pressed)
					current->m_frame = static_cast<int>(sre::current_frame());
				break;
			}
			current = current->m_next;
		}
		break;
	}
	break;
	case EVENT_TOUCH:
		ALOG("Touching... %f %d", ev.touch.pressure, ev.touch.pressed);
		ev.touch.delta.print();
		ev.touch.uv.println();
		break;
	default:
		return;
	}
}