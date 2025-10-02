# 🤝 Contributing to the Plugin

First and foremost, thank you for considering a contribution! We welcome all kinds of contributions, from new features and bug fixes to documentation improvements and community support. Your help is what keeps this project moving forward.

By participating, you agree to abide by our **[Code of Conduct](https://github.com/BenPyton/ProceduralDungeon?tab=coc-ov-file#readme)**.

## I have a question or an idea

* **Have a question?** For general questions or support, please ask on our [Discord server][discord].
* **Found a bug?** Please [open an issue](https://github.com/BenPyton/proceduralDungeon/issues/new?template=bug_report.yml) and provide a clear description and steps to reproduce the problem.
* **Want to suggest a feature?** Please [open a feature request](https://github.com/BenPyton/ProceduralDungeon/issues/new?template=feature_request.yml) to discuss your idea with the community before you start working on it. This helps prevent duplicated effort.

## Ready to Contribute Code?

If you want to contribute code, we use a specific **Semantic Branching** workflow to keep our releases predictable and our history clean. This process is crucial for managing our codebase effectively.

### **Quick Facts: The Branching Workflow**

Please read this section carefully. It outlines the key rules for your contribution.

* **Do not use `master` as a base for development.**
* **Choose a base branch:** `fixes` (for bugfixes), `minor` (for new features), or `major` (for breaking changes).
* **Create a new feature branch** from only one of the base branches.
* **Rebase your feature branch** onto its base branch before creating a Pull Request (PR). Do not merge the base branch into your feature branch.
* **All feature branches must maintain a linear history.** Avoid merging other branches into your feature branch.
* **Clean up your branch history** before merging. Squash your commits to ensure each resulting commit is a single, logical change. No "WIP" or "unstable" commits.
* **Each commit must be atomic:** one logical change per commit, with tests passing for that commit.
* **Base branches are merged into `master` only for releases.**

### **Branch Purposes**

| Branch | SemVer | Purpose | Compatibility |
| --- | --- | --- | --- |
| `fixes` | x.y.**Z** | Patches and bug fixes | Must be backward *and* forward compatible in the same minor version. |
| `minor` | x.**Y**.z | New and backward-compatible features | Must be safe to update to in the same major version. The end result for users must not change. |
| `major` | **X**.y.z | Breaking changes | Anything that would break existing features. Users will have to modify their project when updating. |

**Example Changes:**

* Adding a private variable → `fixes`
* Adding a new pin to a node → `minor`
* Removing a class → `major`

### **Getting Started with Your Code Contribution**

1. **Fork the repository** and clone it to your local machine.  
  The clone must be done in the `Plugins` folder of an Unreal Engine project.  
  You'll have to regenerate the Visual Studio project file.
2. **Choose the correct base branch** for your change (see the table above).
3. **Create your new branch** with a descriptive name, like `fix/issue-123-bug` or `feat/add-utility-nodes`.

    ```sh
    git fetch origin
    git checkout origin/minor -b feat/add-utility-nodes
    ```

### **Making Your Changes**

* **Write Atomic Commits:** Each commit should represent a single, focused, logical change.
* **Keep Your Branch Small:** Short-lived, focused branches are easier to review and less likely to cause merge conflicts.
* **Test Your Changes:** Make sure your changes leave the test suite green.  
  You can run the `Procedural Dungeon` test suite in `Tools -> Test Automation` (after enabling the `Functional Testing Editor` plugin).  
  It is strongly advised to add unit tests for your changes if suitable.

### **Submitting a Pull Request (PR)**

1. **Rebase Your Branch:** Before opening a PR, rebase your feature branch onto the latest commit of its base branch to integrate upstream changes. This keeps the history linear.

    ```sh
    git fetch origin
    git rebase origin/minor
    ```

2. **Tidy Your Commit History:** Use an interactive rebase to squash or reorganize your commits. This is a critical step to ensure each commit is atomic and meaningful before it is merged.

    ```sh
    git rebase -i origin/minor
    # Follow the interactive prompts to reorder, squash, or fixup commits
    ```

3. **Push and Open the PR:** Once your history is clean, force-push your changes on your fork and open a PR against the **same base branch** you used locally.

    ```sh
    git push --force-with-lease
    ```

4. **Complete the PR Description:** In the PR description, please state the purpose of your changes and their **SemVer impact**. If it's a `major` change, include any necessary migration notes for users.

5. **Sign the CLA**: Once submitted, a **Contributor License Agreement (CLA)** will be presented to you. You must sign it in order for your PR to be reviewed.

6. **Code Review:** Your PR needs code review approval by at least one maintainer before it can be merged.

7. **Update your PR:** Please address any requested changes and update your branch history as needed.

## Releasing and `master`

Releases are handled by maintainers. The appropriate base branches are merged into `master` (in a controlled order), and the releases are tagged with `vX.Y.Z` on `master`.

## Hotfixes, Backports, and Good Practices

This section contains important information for maintainers and advanced contributors.

* **Urgent fixes** should be applied to the `fixes` branch. These changes can then be cherry-picked or re-applied to the `minor` and `major` branches as needed.
* **Keep branches small and focused** to reduce merge conflicts.
* **Use feature flags** for long-lived or partially complete features.
* **Protect base branches** with branch protection rules to enforce passing CI checks and at least one code review.

## Support

If you're unsure which base branch to use, or if you have any questions about the workflow, please don't hesitate to [open an issue](https://github.com/BenPyton/ProceduralDungeon/issues/new?template=BLANK_ISSUE) or ask the maintainers on our [Discord server][discord].

[discord]: https://discord.gg/YE2dPda2CC
