## Assumptions
1. File names, user_ids, group_ids, passwords etc do not contain space
2. Tracker 0 is hard coded in client.cpp
3. My data is non-persistent.

## Implementation
1. Download happens from one peer completely.
2. I was not able to implement download using chunks.
3. All the commands are implemented except stop_sharing of client.
4. Only single tracker is implemented.
5. Multiple peers can download at a time.
