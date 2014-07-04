Create Patch:

git format-patch fromHASH...toHASH --stdout > ThePatchName.patch

Apply Patch:

* Extract AgentEmbedded to Platform/ExternalPackages/

Check changes:

* git apply --stat ThePatchName.patch

Test patch:

* git apply --check ThePatchName.patch

Apply patch:

* git am --signoff < ThePatchName.patch
