#include "camera.h"
#include <util/vec_convert.h>
#include <render/render_manager.h>

using namespace std;

Camera::Camera(string identifier, int priority) : _identifier(identifier), _priority(priority) { RenderManager::Registry::__register(*this); }
Camera::~Camera() { RenderManager::Registry::__unregister(*this); }

const string& Camera::identifier() const { return _identifier; }
int Camera::priority() const { return _priority; }
void Camera::set_priority(int priority) { _priority = priority; RenderManager::inst().on_camera_order_changed(); }

const fvec2& Camera::position() const { return _position; }

sf::View Camera::as_view() const { return sf::View(to_sfvec(_position), sf_fvec2(_size / RenderManager::inst()._aspect, _size)); }