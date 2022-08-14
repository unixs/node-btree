const { GLIB_VERSION } = require("../lib");

describe("GLIB info", () => {
  it("field exists", () => {
    expect(GLIB_VERSION).toBeTruthy();
  });

  it("has full version", () => {
    expect(GLIB_VERSION.FULL).toMatch(/^\d{1,}\.\d{1,}\.\d{1,}$/);
  });

  it("has major version", () => {
    expect(typeof GLIB_VERSION.MAJOR).toBe("number");
  });

  it("has minor version", () => {
    expect(typeof GLIB_VERSION.MINOR).toBe("number");
  });

  it("has patch version", () => {
    expect(typeof GLIB_VERSION.PATCH).toBe("number");
  });

  describe("hasGTreeNode()", () => {
    it("is function", () => {
      expect(typeof GLIB_VERSION.hasGTreeNode).toBe("function");
    });

    it("return boolean", () => {
      expect(typeof GLIB_VERSION.hasGTreeNode()).toBe("boolean");
    });

    it("return false if version < 2.68", () => {
      let expected = true;

      if (GLIB_VERSION.MAJOR == 2) {
        if (GLIB_VERSION.MINOR < 68) {
          expected = false;
        }
      }
      else if (GLIB_VERSION.MAJOR < 2) {
        expected = false;
      }

      expect(GLIB_VERSION.hasGTreeNode()).toBe(expected);
    });
  });
});
