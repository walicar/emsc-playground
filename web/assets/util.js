function getProjectName() {
    const url = new URL(window.location.href);
    return url.pathname.split("/").slice(1)[0];
}