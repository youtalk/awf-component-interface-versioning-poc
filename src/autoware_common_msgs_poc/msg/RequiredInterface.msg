string ns
string declared_topic    # spec-declared canonical topic (Spec::name); the admission matching key
string resolved_topic    # remap-resolved wire topic (subscription->get_topic_name()); == declared_topic when not remapped
string type_name
uint16 accept_major_min
uint16 accept_major_max
uint16 min_minor
