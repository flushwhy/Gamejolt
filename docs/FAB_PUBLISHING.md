# Fab publishing checklist

This document prepares the repository and listing content. It does **not** mean the product has been approved by Fab or that every targeted Unreal Engine version has been tested.

Official references:
- [Fab Publisher Get Started](https://dev.epicgames.com/documentation/fab/publisher-get-started-in-fab)
- [Publishing Assets for Sale or Free Download](https://dev.epicgames.com/documentation/fab/publishing-assets-for-sale-or-free-download-in-fab)
- [Fab Technical Requirements](https://www.fab.com/o/technical-requirements)
- [Asset File Format and Structure Requirements](https://dev.epicgames.com/documentation/fab/asset-file-format-and-structure-requirements-in-fab)

## Repository and package

- [ ] Confirm the plugin version in `GameJolt.uplugin`, `CHANGELOG.md`, the release tag, and the uploaded archive all match.
- [ ] Build the plugin from source in a clean C++ project on each Unreal Engine version you intend to advertise as tested.
- [ ] Smoke-test login, session open/ping/close, trophy fetch, score submission, and global/user data operations.
- [ ] Package only the plugin folder and required files; exclude local project files, caches, logs, and unrelated repository content.
- [ ] Extract the final archive into a fresh project's `Plugins/GameJolt/` directory and follow the README installation steps.
- [ ] Verify the final archive has the expected folder layout and contains `GameJolt.uplugin`, `Source/`, and any other required runtime files.
- [ ] Ensure examples, screenshots, logs, and test projects contain no user tokens, private keys, or other secrets.
- [ ] Keep the repository's current license unchanged in this work. Before publishing, confirm you have the rights to distribute every included file and choose Fab's listing license only after checking compatibility with those rights.

## Listing setup

- [ ] Complete the Fab publisher profile, creator code, and required verification/payout details.
- [ ] Choose the appropriate Fab category and pricing/licensing options.
- [ ] Use the listing copy in [FAB_LISTING_COPY.md](FAB_LISTING_COPY.md) as a starting point; edit it to match the exact package you upload.
- [ ] Add a clear thumbnail and at least one gallery image, video, or 3D preview, as required by Fab.
- [ ] Prepare original screenshots of the plugin's Project Settings, representative Blueprint nodes, and a working example. Do not imply a feature or engine version is verified unless it is.
- [ ] Add relevant tags, concise requirements, install instructions, documentation link, support link, and migration notes.
- [ ] Check Fab's current technical requirements, content guidelines, naming/trademark restrictions, and license terms before submission.
- [ ] Preview the listing, submit it for review, resolve any requested changes, then publish after approval.

## Compatibility wording

The project's intended target is Unreal Engine **4.20–4.27** and **5.0–5.8**. Do not describe every minor version as tested until a build and basic runtime smoke test has passed for that version. If testing is only available on selected versions, name those tested versions and describe the others as targets or unverified.

## Before calling it release-ready

- [ ] A clean install/build has been completed on the versions claimed as supported.
- [ ] The packaged archive has been tested independently of the development checkout.
- [ ] The documentation and listing match the actual release.
- [ ] The Fab listing has passed review and is live.

