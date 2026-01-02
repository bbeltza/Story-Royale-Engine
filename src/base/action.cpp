#include <Base/Action.hpp>

using namespace sre;

Action* Action::head_ptr = NULL;
ConnectionHandle Action::sc_mouse;
ConnectionHandle Action::sc_keyboard;
ConnectionHandle Action::sc_touch;

Action::Action(): m_next(head_ptr)
{
	if (!sc_mouse.Connected())
	{
		sc_mouse = Input::MouseButton.Connect(sc_handlemouse, NULL);
		sc_keyboard = Input::KeyEvent.Connect(sc_handlekeyboard, NULL);
		//sc_touch = Input::FingerTouch.Connect(sc_handletouch, NULL);
	}

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

void Action::sc_handlekeyboard(void*, void*, const Key* ev)
{
	if (ev->repeat) return;

	const int scancode_val = ev->scanCode & C_SCANCODE_MASK;
	const int keycode_val = ev->keyCode & C_KEYCODE_MASK;
	const int incrementer = ev->pressed ? 1 : -1;

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
		else if (ev->pressed)
			current->m_frame = static_cast<int>(sre::current_frame());
		
		END:
		current = current->m_next;
	}
}

void Action::sc_handlemouse(void*, void*, const MouseButton* ev)
{
	int incrementer = ev->pressed ? 1 : -1;

	auto current = head_ptr;
	while (current)
	{
		for (unsigned val : current->m_inputs)
		{
			if ((val & C_MOUSE_MASK) != ev->button) continue;

			current->m_counter += incrementer;
			if (!current->m_counter)
				current->m_frame = -1;
			else if (ev->pressed)
				current->m_frame = static_cast<int>(sre::current_frame());
			break;
		}
		current = current->m_next;
	}
}