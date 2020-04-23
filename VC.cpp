#include "VC.h"

VC get_vc() {
	static VC vc(false);
	static bool init = false;
	if (init) {
		return vc;
	} else {
		init = true;
		vc = VC(true);
		return vc;
	}
}
