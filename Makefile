
requirevar-%:
	@if test "$($(*))" = "" ; then \
		echo "Required variable $(*) not set"; \
		exit 1; \
	fi

all:
	@cd vendor/builds/boost-1.58/; \
	tar -xf lib-linux-x86-32.tar.bz2; \
	tar -xf lib-linux-x86-64.tar.bz2; \
	echo "vendor fix done"
	scons

clean:
	scons -c
	rm -rf vendor/builds/boost-1.58/lib-linux-*/

# Usage: make remotebuild \
#     REMOTEHOST="localhost" REMOTEPORT="2222" REMOTEUSER="erik" \
#     REMOTEDEST="~" [REMOTEMKARGS="..."]
# 
remotebuild: requirevar-REMOTEHOST requirevar-REMOTEPORT \
		requirevar-REMOTEUSER requirevar-REMOTEDEST
	rsync --rsh="ssh -p $(REMOTEPORT)" -av --delete --exclude=.git \
		$(PWD) $(REMOTEUSER)@$(REMOTEHOST):$(REMOTEDEST)
	ssh -t -p $(REMOTEPORT) $(REMOTEUSER)@$(REMOTEHOST) \
		"cd $(REMOTEDEST)/`basename $(PWD)` && make $(REMOTEMKARGS)"

