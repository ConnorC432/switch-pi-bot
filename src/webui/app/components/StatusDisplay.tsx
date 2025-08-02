'use client';

import React, { useState, useEffect } from 'react';
import {Box, Typography, Card, CircularProgress} from "@mui/material";


interface Status {
  status: 'Starting' | 'Running' | 'Error' | 'Finished';
  currentGame?: { name: string };
  currentProgram?: { name: string };
}


// Fetch status from the API
const fetchStatus = async (): Promise<Status> => {
  const response = await fetch('/api/programs/get-program-status');
  if (!response.ok) throw new Error('Failed to fetch status');
  return response.json();
};

const StatusDisplay: React.FC = () => {
  const [status, setStatus] = useState<Status | null>(null);
  const [loading, setLoading] = useState<boolean>(true);

  useEffect(() => {
    const fetchAndUpdateStatus = async () => {
      try {
        const data = await fetchStatus();
        setStatus(data);
      } catch (error) {
        console.error('Error fetching status:', error);
      } finally {
        setLoading(false);
      }
    };


    fetchAndUpdateStatus();
    const interval = setInterval(fetchAndUpdateStatus, 500);

    return () => clearInterval(interval);
  }, []);


  if (loading) {
    return (
      <Box sx={{ display: 'flex', justifyContent: 'center', mt: 4 }}>
        <CircularProgress />
      </Box>
    );
  }


  if (!status || !['Starting', 'Running', 'Error', 'Finished'].includes(status.status)) {
    return null;
  }

  let backColor = '#0ab9e6';
  if (status.status === 'Finished') {
    backColor = '#00e676';
  } else if (status.status === 'Error') {
    backColor = '#ff3c28';
  }

  return (
    <Card variant="outlined" sx={{ mt: 2, width: '100%', maxWidth: 480, borderRadius: 2 }}>
      <Box sx={{ p: 2, textAlign: 'center', backgroundColor: backColor  }}>
        <Typography variant="h6">
          Current Status: {status.status}
        </Typography>
      </Box>
      {/*<Divider />
      <Box sx={{ p: 2, backgroundColor: background.paper }}>
        {status.currentGame && (
          <Typography variant="body2">
            Game: {status.currentGame.name}
          </Typography>
        )}

        {status.currentProgram && (
          <Typography variant="body2">
            Program: {status.currentProgram.name}
          </Typography>
        )}
      </Box>*/}
    </Card>
  );
};

export default StatusDisplay;
