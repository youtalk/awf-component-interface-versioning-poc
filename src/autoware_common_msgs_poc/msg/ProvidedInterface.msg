string ns
string interface_name   # spec-declared interface name (Spec::name = topic/service/action name); the admission matching key
string resolved_name    # remap-resolved FQN (get_topic_name() / get_service_name()); == interface_name when not remapped
string type_name
uint16 major
uint16 minor
uint16 patch
