string ns
string declared_topic    # spec-declared canonical topic (Spec::name); the admission matching key
string resolved_topic    # remap-resolved wire topic (publisher->get_topic_name()); == declared_topic when not remapped
string type_name
uint16 major
uint16 minor
uint16 patch
