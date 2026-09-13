const MERMAID_ICONS = {
  zoomIn:
    '<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="2" stroke="currentColor" aria-hidden="true"><path stroke-linecap="round" stroke-linejoin="round" d="M12 5v14M5 12h14" /></svg>',
  zoomOut:
    '<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="2" stroke="currentColor" aria-hidden="true"><path stroke-linecap="round" stroke-linejoin="round" d="M5 12h14" /></svg>',
  reset:
    '<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="2" stroke="currentColor" aria-hidden="true"><path stroke-linecap="round" stroke-linejoin="round" d="M4 12a8 8 0 1 0 2.343-5.657M4 4v4h4" /></svg>',
  fullscreen:
    '<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="2" stroke="currentColor" aria-hidden="true"><path stroke-linecap="round" stroke-linejoin="round" d="M9 3H5a2 2 0 0 0-2 2v4m16 0V5a2 2 0 0 0-2-2h-4m0 18h4a2 2 0 0 0 2-2v-4M3 15v4a2 2 0 0 0 2 2h4" /></svg>',
  close:
    '<svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" stroke-width="2" stroke="currentColor" aria-hidden="true"><path stroke-linecap="round" stroke-linejoin="round" d="M6 6l12 12M18 6L6 18" /></svg>',
};

const mermaidControllers = new WeakMap();
let tocObserver = null;

document.addEventListener("DOMContentLoaded", async () => {
  setupLocaleToggle();
  setupHeaderCopy();
  setupCodeCopy();
  setupShareButton();
  setupSidebarToggle();
  setupThemeToggle();
  setupTocHighlight();
  await renderMermaidBlocks();
  setupMermaidModal();
});

function getCurrentLocale() {
  return document.documentElement.dataset.dwLocale || window.__DW_PAGE?.defaultLocale || "en";
}

function setupLocaleToggle() {
  const buttons = Array.from(document.querySelectorAll("[data-set-locale]"));
  if (!buttons.length) {
    updatePageMetadata(getCurrentLocale());
    return;
  }
  const availableLocales = window.__DW_PAGE?.availableLocales || ["en"];
  const defaultLocale = window.__DW_PAGE?.defaultLocale || availableLocales[0] || "en";

  const applyLocale = (nextLocale, options = {}) => {
    const { persist = true } = options;
    const locale = availableLocales.includes(nextLocale) ? nextLocale : defaultLocale;
    document.documentElement.dataset.dwLocale = locale;
    document.documentElement.lang = locale === "zh" ? "zh-CN" : "en";
    buttons.forEach((button) => {
      const isActive = button.dataset.setLocale === locale;
      button.dataset.active = isActive ? "true" : "false";
      button.setAttribute("aria-pressed", isActive ? "true" : "false");
    });
    if (persist) {
      try {
        localStorage.setItem("dw-locale", locale);
      } catch (error) {
        console.warn("Failed to persist locale:", error);
      }
    }
    updatePageMetadata(locale);
    setupTocHighlight();
  };

  buttons.forEach((button) => {
    button.addEventListener("click", () => {
      applyLocale(button.dataset.setLocale || defaultLocale);
    });
  });

  window.addEventListener("hashchange", () => {
    const hashLocale = detectLocaleFromHash(availableLocales);
    if (hashLocale) {
      applyLocale(hashLocale, { persist: false });
    }
  });

  applyLocale(detectLocaleFromHash(availableLocales) || getCurrentLocale(), { persist: false });
}

function setupHeaderCopy() {
  document.querySelectorAll("[data-copy-header]").forEach((button) => {
    button.addEventListener("click", async () => {
      const heading = button.closest("[data-header]");
      if (!heading || !heading.id) return;
      const url = `${window.location.origin}${window.location.pathname}#${heading.id}`;
      await navigator.clipboard.writeText(url);
    });
  });
}

function setupCodeCopy() {
  document.querySelectorAll("[data-copy-code]").forEach((button) => {
    button.addEventListener("click", async () => {
      const code = button.parentElement?.querySelector("code");
      if (!code) return;
      await navigator.clipboard.writeText(code.textContent || "");
      const previous = button.textContent;
      button.textContent = "Copied";
      setTimeout(() => {
        button.textContent = previous || "Copy";
      }, 1200);
    });
  });
}

function setupSidebarToggle() {
  const toggle = document.querySelector("[data-sidebar-toggle]");
  const sidebar = document.querySelector(".dw-sidebar-panel");
  if (!toggle || !sidebar) return;
  toggle.addEventListener("click", () => {
    sidebar.classList.toggle("is-open");
  });
}

function setupShareButton() {
  const button = document.querySelector("[data-share-page]");
  if (!button) return;
  button.addEventListener("click", async () => {
    await navigator.clipboard.writeText(window.location.href);
    const previous = button.innerHTML;
    button.textContent = getCurrentLocale() === "zh" ? "已复制" : "Copied";
    setTimeout(() => {
      button.innerHTML = previous;
    }, 1200);
  });
}

function setupThemeToggle() {
  const button = document.querySelector("[data-theme-toggle]");
  if (!button) return;
  button.addEventListener("click", async () => {
    const next = document.documentElement.classList.contains("dark") ? "light" : "dark";
    document.documentElement.classList.remove("light", "dark");
    document.documentElement.classList.add(next);
    document.documentElement.style.colorScheme = next;
    localStorage.setItem("theme", next);
    await renderMermaidBlocks({ force: true });
    setupMermaidModal();
  });
}

function setupTocHighlight() {
  tocObserver?.disconnect();
  const locale = getCurrentLocale();
  const activePanel = document.querySelector(`.dw-toc-panel [data-locale-panel="${locale}"]`);
  const links = Array.from(activePanel?.querySelectorAll("a[href^='#']") || []);
  if (!links.length) return;
  const entries = links
    .map((link) => {
      const target = document.querySelector(link.getAttribute("href"));
      return target ? { link, target } : null;
    })
    .filter(Boolean);
  const observer = new IntersectionObserver(
    (items) => {
      const visible = items
        .filter((item) => item.isIntersecting)
        .sort((a, b) => a.boundingClientRect.top - b.boundingClientRect.top)[0];
      if (!visible) return;
      entries.forEach(({ link }) => {
        link.classList.remove("text-primary", "font-medium");
        if (!link.classList.contains("text-secondary")) {
          link.classList.add("text-secondary");
        }
      });
      const active = entries.find(({ target }) => target === visible.target);
      if (active) {
        active.link.classList.remove("text-secondary");
        active.link.classList.add("text-primary", "font-medium");
      }
    },
    { rootMargin: "-20% 0px -70% 0px", threshold: [0, 1] },
  );
  tocObserver = observer;
  entries.forEach(({ target }) => observer.observe(target));
}

function updatePageMetadata(locale) {
  const titles = window.__DW_PAGE?.titles || {};
  const descriptions = window.__DW_PAGE?.descriptions || {};
  const repoName = window.__DW_PAGE?.repoName || "";
  const title = titles[locale] || titles.en;
  const description = descriptions[locale] || descriptions.en;
  if (title) {
    document.title = repoName ? `${title} | ${repoName} | DeepWiki Export` : title;
  }
  const metaDescription = document.querySelector('meta[name="description"]');
  if (metaDescription && description) {
    metaDescription.setAttribute("content", description);
  }
}

function detectLocaleFromHash(availableLocales) {
  const hash = window.location.hash.replace(/^#/, "");
  if (!hash) return null;
  if (hash.startsWith("zh-") && availableLocales.includes("zh")) return "zh";
  if (hash.startsWith("en-") && availableLocales.includes("en")) return "en";
  return null;
}

async function renderMermaidBlocks(options = {}) {
  const mermaidBlocks = Array.from(document.querySelectorAll("code.language-mermaid"));
  const renderedBlocks = Array.from(document.querySelectorAll(".dw-mermaid-render[data-mermaid-source]"));
  if (!mermaidBlocks.length && !renderedBlocks.length) return;
  const mermaid = await loadMermaid();
  const isDark = document.documentElement.classList.contains("dark");
  const mermaidFontFamily = "ui-sans-serif,-apple-system,system-ui,Segoe UI,Helvetica,Arial,sans-serif";
  mermaid.initialize({
    startOnLoad: false,
    securityLevel: "loose",
    theme: isDark ? "dark" : "neutral",
    flowchart: {
      useMaxWidth: true,
      htmlLabels: true,
    },
    themeVariables: isDark
      ? {
          fontSize: "16px",
          fontFamily: mermaidFontFamily,
        }
      : {
          fontSize: "16px",
          fontFamily: mermaidFontFamily,
          primaryColor: "#ffffff",
          primaryBorderColor: "#d4d4d8",
          primaryTextColor: "#52525b",
          lineColor: "#a1a1aa",
          tertiaryColor: "#fafafa",
          clusterBkg: "#fafafa",
          clusterBorder: "#e4e4e7",
        },
  });
  for (const wrapper of renderedBlocks) {
    if (!options.force && wrapper.querySelector("svg")) continue;
    const source = wrapper.dataset.mermaidSource || "";
    if (!source) continue;
    await renderMermaidIntoWrapper(mermaid, wrapper, source);
  }
  for (const code of mermaidBlocks) {
    const source = code.textContent || "";
    const pre = code.closest("pre");
    if (!pre) continue;
    const wrapper = document.createElement("div");
    wrapper.className = "dw-mermaid-render";
    wrapper.dataset.mermaidSource = source;
    await renderMermaidIntoWrapper(mermaid, wrapper, source);
    pre.replaceWith(wrapper);
  }
}

async function renderMermaidIntoWrapper(mermaid, wrapper, source) {
  const id = `dw-mermaid-${Math.random().toString(36).slice(2)}`;
  try {
    const { svg } = await mermaid.render(id, source);
    const svgElement = createSvgElement(svg);
    if (!svgElement) return;
    mountMermaidViewport(wrapper, svgElement, {
      allowFullscreen: true,
      source,
      modal: false,
    });
  } catch (error) {
    console.error("Mermaid render failed:", error);
  }
}

async function loadMermaid() {
  if (window.mermaid) return window.mermaid;
  const script = document.createElement("script");
  script.src = `${window.__DW_ASSET_PREFIX || "./assets/"}mermaid.min.js`;
  script.defer = true;
  document.head.appendChild(script);
  await new Promise((resolve, reject) => {
    script.onload = resolve;
    script.onerror = reject;
  });
  return window.mermaid;
}

function setupMermaidModal() {
  const modal = ensureMermaidModal();
  const viewer = modal.querySelector("[data-mermaid-modal-viewer]");
  const close = () => closeMermaidModal(modal, viewer);

  if (modal.dataset.bound === "true") return;
  modal.dataset.bound = "true";
  modal.querySelector("[data-mermaid-modal-backdrop]").addEventListener("click", close);
  document.addEventListener("keydown", (event) => {
    if (event.key === "Escape" && modal.classList.contains("is-open")) {
      close();
    }
  });
}

function ensureMermaidModal() {
  let modal = document.querySelector("[data-mermaid-modal]");
  if (modal) return modal;
  modal = document.createElement("div");
  modal.className = "dw-mermaid-modal";
  modal.dataset.mermaidModal = "true";
  modal.innerHTML = `
    <div class="dw-mermaid-modal__backdrop" data-mermaid-modal-backdrop="true"></div>
    <div class="dw-mermaid-modal__dialog" role="dialog" aria-modal="true" aria-label="Expanded Mermaid diagram">
      <div class="dw-mermaid-modal__viewer" data-mermaid-modal-viewer="true"></div>
    </div>
  `;
  document.body.appendChild(modal);
  return modal;
}

function openMermaidModal(modal, viewer, wrapper) {
  const svg = wrapper.querySelector(".dw-mermaid-svg");
  if (!svg) return;
  destroyMermaidViewport(viewer.firstElementChild);
  viewer.innerHTML = "";
  const modalViewport = document.createElement("div");
  const modalSvg = svg.cloneNode(true);
  modalSvg.style.transform = "";
  viewer.appendChild(modalViewport);
  mountMermaidViewport(modalViewport, modalSvg, {
    allowFullscreen: false,
    source: wrapper.dataset.mermaidSource || "",
    modal: true,
    onClose: () => closeMermaidModal(modal, viewer),
  });
  modal.classList.add("is-open");
  document.body.classList.add("dw-modal-open");
}

function closeMermaidModal(modal, viewer) {
  destroyMermaidViewport(viewer.firstElementChild);
  viewer.innerHTML = "";
  modal.classList.remove("is-open");
  document.body.classList.remove("dw-modal-open");
}

function createSvgElement(svgMarkup) {
  const template = document.createElement("template");
  template.innerHTML = svgMarkup.trim();
  return template.content.firstElementChild;
}

function mountMermaidViewport(container, svgElement, options = {}) {
  const { allowFullscreen = true, source = "", modal = false, onClose = null } = options;
  destroyMermaidViewport(container);
  container.innerHTML = "";
  container.dataset.mermaidSource = source;
  container.className = modal ? "dw-mermaid-render dw-mermaid-render--modal" : "dw-mermaid-render";

  const toolbar = document.createElement("div");
  toolbar.className = "dw-mermaid-toolbar";

  const stage = document.createElement("div");
  stage.className = modal ? "dw-mermaid-stage dw-mermaid-stage--modal" : "dw-mermaid-stage";
  const canvas = document.createElement("div");
  canvas.className = "dw-mermaid-canvas";
  svgElement.classList.add("dw-mermaid-svg");
  svgElement.style.maxWidth = "none";
  svgElement.style.maxHeight = "none";
  canvas.appendChild(svgElement);
  stage.appendChild(canvas);

  container.appendChild(toolbar);
  container.appendChild(stage);

  const controls = createMermaidPanZoom(stage, canvas, svgElement);
  toolbar.appendChild(createMermaidControlButton(MERMAID_ICONS.zoomIn, "Zoom in", controls.zoomIn));
  toolbar.appendChild(createMermaidControlButton(MERMAID_ICONS.zoomOut, "Zoom out", controls.zoomOut));
  if (modal && onClose) {
    toolbar.appendChild(createMermaidControlButton(MERMAID_ICONS.close, "Close", onClose));
  } else {
    toolbar.appendChild(createMermaidControlButton(MERMAID_ICONS.reset, "Reset view", controls.reset));
  }
  if (allowFullscreen && !modal) {
    toolbar.appendChild(
      createMermaidControlButton(MERMAID_ICONS.fullscreen, "Open fullscreen", () => {
        const modalRoot = ensureMermaidModal();
        const viewer = modalRoot.querySelector("[data-mermaid-modal-viewer]");
        openMermaidModal(modalRoot, viewer, container);
      }),
    );
  }

  mermaidControllers.set(container, controls);
}

function destroyMermaidViewport(container) {
  if (!container) return;
  const controller = mermaidControllers.get(container);
  if (!controller) return;
  controller.destroy();
  mermaidControllers.delete(container);
}

function createMermaidControlButton(iconMarkup, label, handler) {
  const button = document.createElement("button");
  button.type = "button";
  button.className = "dw-mermaid-control";
  button.setAttribute("aria-label", label);
  button.title = label;
  button.innerHTML = iconMarkup;
  button.addEventListener("click", handler);
  return button;
}

function createMermaidPanZoom(stage, canvas, svg) {
  const dimensions = getSvgDimensions(svg);
  const state = {
    scale: 1,
    minScale: 1,
    maxScale: 5,
    x: 0,
    y: 0,
    dragging: false,
    lastX: 0,
    lastY: 0,
    intrinsicWidth: dimensions.width,
    intrinsicHeight: dimensions.height,
  };

  const resizeObserver =
    typeof ResizeObserver !== "undefined"
      ? new ResizeObserver(() => applyTransform())
      : null;
  resizeObserver?.observe(stage);

  const endDrag = () => {
    state.dragging = false;
    stage.classList.remove("is-panning");
  };

  const beginDrag = (clientX, clientY) => {
    if (stage.dataset.canPan !== "true") return;
    state.dragging = true;
    state.lastX = clientX;
    state.lastY = clientY;
    stage.classList.add("is-panning");
  };

  const handleMouseMove = (event) => {
    if (!state.dragging) return;
    if (stage.dataset.canPan !== "true") return;
    state.x += event.clientX - state.lastX;
    state.y += event.clientY - state.lastY;
    state.lastX = event.clientX;
    state.lastY = event.clientY;
    applyTransform();
  };

  stage.addEventListener(
    "wheel",
    (event) => {
      event.preventDefault();
      zoomTo(state.scale * (event.deltaY < 0 ? 1.12 : 0.89));
    },
    { passive: false },
  );

  stage.addEventListener("mousedown", (event) => {
    if (event.button !== 0) return;
    event.preventDefault();
    beginDrag(event.clientX, event.clientY);
  });

  window.addEventListener("mousemove", handleMouseMove);

  window.addEventListener("mouseup", endDrag);

  applyTransform();

  return {
    zoomIn() {
      zoomTo(state.scale * 1.2);
    },
    zoomOut() {
      zoomTo(state.scale / 1.2);
    },
    reset() {
      state.scale = 1;
      state.x = 0;
      state.y = 0;
      applyTransform();
    },
    destroy() {
      resizeObserver?.disconnect();
      window.removeEventListener("mousemove", handleMouseMove);
      window.removeEventListener("mouseup", endDrag);
      endDrag();
    },
  };

  function zoomTo(nextScale) {
    state.scale = clamp(nextScale, state.minScale, state.maxScale);
    applyTransform();
  }

  function applyTransform() {
    const viewport = getViewportMetrics();
    state.minScale = viewport.minScale;
    state.scale = clamp(state.scale, state.minScale, state.maxScale);
    stage.style.width = `${viewport.width}px`;
    stage.style.height = `${viewport.height}px`;
    canvas.style.width = `${viewport.contentWidth}px`;
    canvas.style.height = `${viewport.contentHeight}px`;
    svg.style.width = "100%";
    svg.style.height = "100%";
    clampOffsets(viewport);
    canvas.style.transform = `translate(${state.x}px, ${state.y}px) scale(${state.scale})`;
    stage.dataset.canPan = canPan(viewport) ? "true" : "false";
  }

  function clampOffsets(viewport) {
    if (!canPan(viewport)) {
      state.x = 0;
      state.y = 0;
      return;
    }
    const maxX = Math.max(0, (viewport.contentWidth * state.scale - viewport.width) / 2);
    const maxY = Math.max(0, viewport.contentHeight * state.scale - viewport.height);
    state.x = clamp(state.x, -maxX, maxX);
    state.y = clamp(state.y, -maxY, 0);
  }

  function canPan(viewport) {
    const scaledWidth = viewport.contentWidth * state.scale;
    const scaledHeight = viewport.contentHeight * state.scale;
    return scaledWidth > viewport.width + 1 || scaledHeight > viewport.height + 1;
  }

  function getViewportMetrics() {
    const host = stage.parentElement?.parentElement;
    const availableWidth = Math.max(host?.clientWidth || stage.clientWidth || 0, 1);
    const availableHeight = stage.classList.contains("dw-mermaid-stage--modal")
      ? Math.max(window.innerHeight * 0.82, 320)
      : Math.max(window.innerHeight * 0.7, 280);
    const scaleToFit = Math.min(availableWidth / state.intrinsicWidth, availableHeight / state.intrinsicHeight);
    const fitScale = Number.isFinite(scaleToFit) && scaleToFit > 0 ? scaleToFit : 1;
    const targetCoverage = stage.classList.contains("dw-mermaid-stage--modal") ? 0.94 : 0.82;
    const preferredScale = Math.max(fitScale, (availableWidth * targetCoverage) / state.intrinsicWidth);
    const contentWidth = Math.max(1, state.intrinsicWidth * preferredScale);
    const contentHeight = Math.max(1, state.intrinsicHeight * preferredScale);
    return {
      width: Math.min(availableWidth, contentWidth),
      height: Math.min(availableHeight, contentHeight),
      contentWidth,
      contentHeight,
      minScale: Math.min(1, fitScale / preferredScale),
    };
  }
}

function clamp(value, min, max) {
  return Math.min(Math.max(value, min), max);
}

function guessSvgAspectRatio(svg) {
  const width = Number(svg.getAttribute("width")) || svg.viewBox?.baseVal?.width || svg.getBoundingClientRect().width || 1;
  const height =
    Number(svg.getAttribute("height")) || svg.viewBox?.baseVal?.height || svg.getBoundingClientRect().height || 0.6;
  return height / width;
}

function getSvgDimensions(svg) {
  const viewBox = svg.viewBox?.baseVal;
  const width = Number(svg.getAttribute("width")) || viewBox?.width || svg.getBoundingClientRect().width || 1;
  const height = Number(svg.getAttribute("height")) || viewBox?.height || svg.getBoundingClientRect().height || 1;
  return {
    width: Math.max(width, 1),
    height: Math.max(height, 1),
  };
}
