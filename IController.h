#ifndef ICONTROLLER_H
#define ICONTROLLER_H

class IController {
	public:
		enum ControllerType {
			HOST_CONTROLLER,
			PHYS_CONTROLLER
		};

		enum SyncDirection {
			FROM_CONTROLLER,
			TO_CONTROLLER,
		};

		IController(ControllerType type);
		virtual ~IController();

		enum ControllerType getType(void);
		virtual void sync(SyncDirection dir) = 0;

	private:
		enum ControllerType type;
};

#endif /* ICONTROLLER_H */
