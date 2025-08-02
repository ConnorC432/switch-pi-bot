"use client";

import * as React from "react";

export default function CaptureStream() {
  const [captureSrc, setCaptureSrc] = React.useState<string>("");

  React.useEffect(() => {
    if (typeof window !== "undefined") {
      const host = window.location.hostname;
      setCaptureSrc(`http://${host}:8080/stream`);
    }
  }, []);

  return (
    <div
      style={{
        position: "fixed",
        top: "64px", // Offset by the height of the header (64px)
        left: 0,
        width: "100vw",
        height: "calc(100vh - 64px)", // Subtract header height from viewport height
        backgroundColor: "black", // Black background to simulate the borders
        display: "flex",
        justifyContent: "center",
        alignItems: "center",
      }}
    >
      <img
        src={captureSrc}
        alt="Capture Card Unavailable"
        style={{
          width: "100%",
          height: "100%",
          objectFit: "contain", // Maintain aspect ratio without cropping
          maxWidth: "100vw",  // Ensure it doesn't exceed the width of the viewport
          maxHeight: "100vh", // Ensure it doesn't exceed the height of the viewport
        }}
        loading="eager"
      />
    </div>
  );
}
