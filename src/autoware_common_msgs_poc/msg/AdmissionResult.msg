# Admission verdict for one consumer<-provider interface pairing.
# The verdict is a single unified code: 0 = accepted, non-zero = IF incompatibility.
# accepted == (code == ACCEPTED); the human-readable reason is derivable from the code
# off-wire, and the concrete versions are already carried in the broadcast InterfaceManifest.
#
# In the production design this `code` is replaced by the Error Code Foundation mechanism:
# it becomes the unified error code raised on the reserved IF-incompatibility domain, so the
# admission verdict reuses the same code space, off-wire canonical/name derivation, and
# downstream MRM wiring as every other Autoware error. The constants below are a minimal,
# self-contained PoC stand-in for that domain's values.
uint16 ACCEPTED = 0
uint16 MAJOR_MISMATCH = 1
uint16 MINOR_MISMATCH = 2
uint16 TOPIC_MISMATCH = 3   # version-compatible, but remap left provider/consumer on disjoint wire topics
uint16 NO_PROVIDER = 4      # deploy-time only: a required interface has no provider in the composed set (the runtime observe-mode handshake never emits this — a provider may simply not have started yet)

string consumer_node
string interface_name
string provider_node
uint16 code
