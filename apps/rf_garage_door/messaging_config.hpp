#ifndef MESSAGING_CONFIG_HPP_
#define MESSAGING_CONFIG_HPP_


//static constexpr uint8_t NODEID = 11;

#define FOR_ALL_INCOMING_MESSAGES(ACTION) \
	ACTION( SwitchRequest )

#endif //MESSAGING_CONFIG_HPP_
