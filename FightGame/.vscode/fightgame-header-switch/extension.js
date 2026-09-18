const vscode = require("vscode");
const fs = require("fs");
const path = require("path");

const HEADER_EXT = /\.(h|hh|hpp|hxx|inc)$/i;
const SOURCE_EXT = /\.(cpp|cc|cxx|c\+\+|c)$/i;

function resolvePairPath(filePath, workspaceRoot) {
  const norm = filePath.replace(/\\/g, "/");
  const root = workspaceRoot.replace(/\\/g, "/");
  if (!norm.startsWith(root + "/")) {
    return null;
  }

  const rel = norm.slice(root.length + 1);
  let targetRel = null;

  if (rel.startsWith("include/") && HEADER_EXT.test(rel)) {
    const rest = rel.slice("include/".length);
    targetRel = "Source/" + rest.replace(HEADER_EXT, ".cpp");
  } else if (rel.startsWith("Source/") && SOURCE_EXT.test(rel)) {
    const rest = rel.slice("Source/".length);
    targetRel = "include/" + rest.replace(SOURCE_EXT, ".h");
  }

  if (!targetRel) {
    return null;
  }

  const target = path.join(workspaceRoot, ...targetRel.split("/"));
  if (!fs.existsSync(target)) {
    return null;
  }

  return target;
}

function activate(context) {
  context.subscriptions.push(
    vscode.commands.registerCommand("fightgame.switchHeaderSource", async () => {
      const editor = vscode.window.activeTextEditor;
      if (!editor) {
        return;
      }

      const folder = vscode.workspace.getWorkspaceFolder(editor.document.uri);
      if (!folder) {
        vscode.window.showWarningMessage("FightGame switch: workspace folder not found.");
        return;
      }

      const target = resolvePairPath(editor.document.uri.fsPath, folder.uri.fsPath);
      if (!target) {
        vscode.window.showWarningMessage(
          "Didn't find a corresponding file (include/ ? Source/ with the same subpath)."
        );
        return;
      }

      const doc = await vscode.workspace.openTextDocument(target);
      await vscode.window.showTextDocument(doc);
    })
  );
}

function deactivate() {}

module.exports = { activate, deactivate };
